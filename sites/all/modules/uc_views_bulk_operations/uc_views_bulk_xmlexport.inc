<?php
/**
 * @file
 * Export class to handle exporting orders to FexEx Ship Manager software
 */

/**
 * Class to handle exporting Ubercart orders to an XML file
 *
 *
 * @author rglunt
 */
class XMLExport {
  
  public function export($selectedOrders) {
    $xml = new SimpleXMLElement('<Orders></Orders>');
    
    foreach ($selectedOrders as $orderId) {
      $order = uc_order_load($orderId);
      
      $orderTag = $xml->AddChild('Order');
      $orderTag->addAttribute('Number', $order->order_id);
      $orderTag->addAttribute('DateOrdered', date('m/d/Y', $order->created));
      $orderTag->addAttribute('Status', $order->order_status);
      $orderTag->AddChild('OrderTotal', number_format($order->order_total, 2));
      $custTag = $orderTag->AddChild('Customer');
      $custTag->addAttribute('Email', $order->primary_email);
      
      $addrTag = $orderTag->AddChild('Address');
      $addrTag->addAttribute('Type', 'Billing');
      $addrTag->addChild('FirstName', $order->billing_first_name);
      $addrTag->addChild('LastName', $order->billing_last_name);
      $addrTag->addChild('Company', $order->billing_company);
      $addrTag->addChild('Line1', $order->billing_street1);
      $addrTag->addChild('Line2', $order->billing_street2);
      $addrTag->addChild('City', $order->billing_city);
      $addrTag->addChild('State', uc_get_zone_code($order->billing_zone));
      $addrTag->addChild('ZipCode', $order->billing_postal_code);
      $country = uc_get_country_data(array('country_id' => $order->billing_country));
      $addrTag->addChild('Country', $country[0]['country_iso_code_3']);      
      $addrTag->addChild('Phone', $order->billing_phone);
      
      $addrTag = $orderTag->AddChild('Address');
      $addrTag->addAttribute('Type', 'Shipping');
      $addrTag->addChild('FirstName', $order->delivery_first_name);
      $addrTag->addChild('LastName', $order->delivery_last_name);
      $addrTag->addChild('Company', $order->delivery_company);
      $addrTag->addChild('Line1', $order->delivery_street1);
      $addrTag->addChild('Line2', $order->delivery_street2);
      $addrTag->addChild('City', $order->delivery_city);
      $addrTag->addChild('State', uc_get_zone_code($order->delivery_zone));
      $addrTag->addChild('ZipCode', $order->delivery_postal_code);
      $country = uc_get_country_data(array('country_id' => $order->delivery_country));
      $addrTag->addChild('Country', $country[0]['country_iso_code_3']);
      $addrTag->addChild('Phone', $order->delivery_phone);
      
      $salesTag = $orderTag->addChild('Sales');
      
      $shippingTotal  = 0;
      $shippingMethod = '';
      $taxTotal       = 0;
      $taxRate        = 0;

      foreach ($order->line_items as $item) {
        if ($item['type'] == 'shipping') {
          $shippingTotal += $item['amount'];
          $shippingMethod = $item['title'];
        }
        else if ($item['type'] == 'tax') {
          $taxTotal += $item['amount'];
          $taxRate = $item['data']['tax_rate'];
        }
      }
      
      $query = "SELECT * FROM uc_packages WHERE order_id = %d";
      $result = db_query($query, $order->order_id);
      while ($data = db_fetch_object($result)) {
        $package = uc_shipping_package_load($data->package_id);
        
        foreach ($package->products as $product) {
          $lineItemTag = $salesTag->addChild('LineItem');
          $lineItemTag->addChild('SKU', $product->model);
          $lineItemTag->addChild('Quantity', $product->qty);
          $lineItemTag->addChild('Price', $product->price);

          $node = node_load($product->nid);
          if (isset($node->taxonomy[JOYCONE_TAX_EXEMPT_TAXONOMY])) {
            $lineItemTag->addChild('Tax', 0);
          } else {
            $lineItemTag->addChild('Tax', $product->price * $product->qty * $taxRate);
          }

          if ($package->warehouse == 'Hermitage Warehouse') {
            $lineItemTag->addChild('Warehouse', '1');
          }
          else if ($package->warehouse == 'Flagstaff Warehouse') {
            $lineItemTag->addChild('Warehouse', '3');
          }          
          else {
            $lineItemTag->addChild('Warehouse', $package->warehouse);
          }
        }
      }
      $shipTag = $orderTag->AddChild('Shipping');
      $shipTag->addAttribute('ShippingTotal', $shippingTotal);
      $shipTag->addChild('Method', $shippingMethod);
      
      $paymentTag = $orderTag->AddChild('Payment');
      $paymentTag->addChild('Method', $order->payment_method);
      
      $taxTag = $orderTag->addChild('TaxRate', $taxRate* 100);
      // $taxTag->addAttribute('TaxTotal', $taxTotal);
    }
    
    $xmlFile = file_directory_path() . '/export.xml';
    
    $dom = new DOMDocument();
    $dom->preserveWhiteSpace = false;
    $dom->formatOutput = false;
    $dom->loadXML($xml->asXML());
    $dom->save($xmlFile);
    file_transfer($xmlFile, array('Content-Type: application/text; charset=utf-8', 'Content-Length: ' . filesize($xmlFile), 'Content-Disposition: attachment; filename="export.xml"'));
  }

}