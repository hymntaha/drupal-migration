<?php

/**
 * Implementation of CCK's hook_widget_settings($op = 'form')
 *
 * @param array $widget
 * @return array
 */
function labelfilefield_widget_settings_form($widget) {
  $form = module_invoke('filefield', 'widget_settings', 'form', $widget);
  
  $form['label'] = array(
    '#type'          => 'textfield',
    '#maxlength'     => '255',
    '#title'         => t('Label'),
    '#description'   => t('Provide a label. The will be the default label for files'),
    '#default_value' => !empty($widget['label']) ? $widget['label'] : '',
  );
  
  return $form;
}

/**
 * Implementation of CCK's hook_widget_settings($op = 'save').
 *
 * @param array $widget
 * @return array
 */
function labelfilefield_widget_settings_save($widget) {
  $filefield_settings = module_invoke('filefield', 'widget_settings', 'save', $widget);
  return array_merge($filefield_settings, array('label'));
}

/**
 * Implementation of CCK's hook_widget_settings($op = 'validate').
 *
 * @param array $widget
 */
function labelfilefield_widget_settings_validate($widget) {  
  module_invoke('filefield', 'widget_settings', 'validate', $widget);
  
  // Check the URL
  if (!empty($widget['label'])) {
    $widget['label'] = check_url($widget['label']);
  }
}

/**
 * Element #value_callback function.
 *
 * @param array $element
 * @param mixed $edit
 * @return array
 */
function labelfilefield_widget_value($element, $edit = FALSE) {
  $item = filefield_widget_value($element, $edit);
  if ($edit) {
    $item['label'] = isset($edit['data']['label']) ? $edit['data']['label'] : '';
  }
  else {
    $item['label'] = '';
  }
  return $item;
}

/**
 * Element #process callback function.
 *
 * @param array $element
 * @param array $edit
 * @param arrayreference $form_state
 * @param array $form
 * @return array
 */
function labelfilefield_widget_process($element, $edit, &$form_state, $form) {
  $file = $element['#value'];
  $element = filefield_widget_process($element, $edit, $form_state, $form);

  // add some help text, telling user the URL field will be viewable after the file is supplied 
  $element['upload']['#description'] = t('Once an File is uploaded, you will ba able to supply a label for it.') . '<br />' . $element['upload']['#description'];
   
  $field = content_fields($element['#field_name'], $element['#type_name']);
  $element['#theme'] = 'labelfilefield_widget_item';
  
  $element['data']['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $file['data']['label'] ? $file['data']['label'] : $field['widget']['label'],
    '#description' => t('This is the label for the file'),
    '#maxlength' => 255,
  );
  
  return $element;
}

/**
 * FormAPI theme function. Theme the output of a file field.
 *
 * @param arrayreference $element
 * @return string
 */
function theme_labelfilefield_widget($element) {
  drupal_add_css(drupal_get_path('module', 'filefield') .'/filefield.css');
  return theme('form_element', $element, $element['#children']);
}
