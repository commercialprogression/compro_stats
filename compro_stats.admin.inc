<?php

/**
 * @file
 * Administration callbacks for the module.
 */

/**
 * Menu callback function for stats administration.
 */
function compro_stats_admin_settings() {
  $form = array();

  $form['compro_stats_blocks'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Make the following blocks available'),
    '#options' => _compro_stats_get_blocks(),
    '#default_value' => variable_get('compro_stats_blocks', array()),
    '#description' => t('These blocks will be made available in places
      such as the blocks admin interface and the homebox layout.'),
  );

  return system_settings_form($form);
}
