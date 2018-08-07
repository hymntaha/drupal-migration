<?php
// $Id: labelfilefield_formatter.inc,v 1.4 2009/08/27 19:50:30 johnfyoung Exp $
/**
  * @file
  *
  * labelfilefield formatter code
  * @author John Young <john@codeandcreative.com>
  */
function theme_labelfilefield_formatter_filelabel($element) {
  // Inside a view $element may contain null data. In that case, just return.
  if (empty($element['#item']['fid'])) {
    return '';
  }

  $field = content_fields($element['#field_name']);
  $item = $element['#item'];

  $item['data']['label'] = _labelfilefield_format_set_attribute('label', $item, $field);

  $class = 'labelfilefield labelfilefield-'. $field['field_name'];
  return theme('labelfilefield_file', $item, array('class' => $class), TRUE, $item['data']['label']);
}
