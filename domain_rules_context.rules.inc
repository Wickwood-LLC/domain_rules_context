<?php

/**
 * UI for Domain ID.
 */
class DomainIDRulesDataUI extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array($name => isset($info['default value']) ? $info['default value'] : NULL);

    $form[$name] = array(
      '#type' => 'select',
      '#title' => t('Domain'),
      '#default_value' => $settings[$name],
      '#options' => domain_rules_context_domains_options_list(),
      '#required' => TRUE,
    );
    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => t('(Domain id: @id)', array('@id' => $value))),
      '#attributes' => array('class' => array('domain-id')),
    );
  }
}

/**
 * UI for Domain.
 */
class DomainRulesDataUI extends RulesDataUI implements RulesDataDirectInputFormInterface {

  public static function getDefaultMode() {
    return 'input';
  }

  public static function inputForm($name, $info, $settings, RulesPlugin $element) {
    $settings += array(
      $name =>
        isset($info['default value']) ?
        $info['default value'] :
        array(
          'subdomain' => NULL, 'sitename' => NULL,
          'scheme' => NULL, 'valid' => NULL, 'weight' => NULL,
          'is_default' => NULL, 'machine_name' => NULL, 'x_type' => NULL,
        )
    );

    if (!variable_get('domain_allow_non_ascii', FALSE)) {
      $description = t('Must contain only dots, lowercase alphanumeric characters, dashes and a colon (if using alternative ports).');
    }
    else {
      $description = t('Must contain only dots, lowercase alphanumeric and ASCII characters, dashes and a colon (if using alternative ports).');
    }

    $form[$name]['subdomain'] = array(
      '#type' => 'textfield',
      '#title' => t('Domain'),
      '#size' => 40,
      '#maxlength' => 80,
      '#required' => TRUE,
      '#default_value' => $settings[$name]['subdomain'],
      '#description' => t('The allowed domain, using the full <em>path.example.com</em> format.') . '<br />' . t('Leave off the http:// and the trailing slash and do not include any paths.') . '<br />' . $description
    );
    $form[$name]['machine_name'] = array(
      '#type' => 'machine_name',
      '#machine_name' => array(
        'source' => array('subdomain'),
        'exists' => 'domain_check_machine_name',
      ),
      '#default_value' => $settings[$name]['machine_name'],
    );
    $form[$name]['sitename'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 40,
      '#maxlength' => 80,
      '#required' => TRUE,
      '#default_value' => $settings[$name]['sitename'],
      '#description' => t('The human-readable name of this domain.')
    );
    $form[$name]['scheme'] = array(
      '#type' => 'radios',
      '#title' => t('Domain URL scheme'),
      '#required' => TRUE,
      '#options' => array('http' => 'http://', 'https' => 'https://'),
      '#default_value' => !empty($settings[$name]['scheme']) ? $settings[$name]['scheme'] : 'http',
      '#description' => t('The URL scheme for accessing this domain.')
    );
    $form[$name]['valid'] = array(
      '#type' => 'radios',
      '#title' => t('Domain status'),
      '#required' => TRUE,
      '#options' => array(1 => t('Active'), 0 => t('Inactive')),
      '#default_value' => isset($settings[$name]['valid']) ? $settings[$name]['valid'] : 1,
      '#description' => t('Must be set to "Active" for users to navigate to this domain.')
    );

    $next_weight = 0;
    foreach (domain_domains() as $record) {
      if ($record['weight'] > $next_weight) {
        $next_weight = $record['weight'];
      }
    }
    $form[$name]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#required' => TRUE,
      '#delta' => count(domain_domains()) + 1,
      '#default_value' => isset($settings[$name]['weight']) ? $settings[$name]['weight'] : $next_weight + 1,
      '#description' => t('The sort order for this record. Lower values display first.'),
    );
    $form[$name]['is_default'] = array(
      '#type' => 'checkbox',
      '#title' => t('Default domain'),
      '#default_value' => isset($settings[$name]['is_default']) ? $settings[$name]['is_default'] : 0,
      '#description' => t('If a URL request fails to match a domain record, the settings for this domain will be used.'),
    );

    return $form;
  }

  public static function render($value) {
    return array(
      'content' => array('#markup' => t('(Domain @sitename)', array('@sitename' => $value['sitename']))),
      '#attributes' => array('class' => array('domain')),
    );
  }
}


/**
 * Implements hook_rules_data_info().
 */
function domain_rules_context_rules_data_info() {
  return array(
    'domain_id' => array(
      'label' => t('Domain ID'),
      'group' => t('Domain'),
      'ui class' => 'DomainIDRulesDataUI',
      'parent' => 'integer',
      'token type' => 'rules_domain_id',
    ),
    'domain' => array(
      'label' => t('Domain'),
      'group' => t('Domain'),
      'ui class' => 'DomainRulesDataUI',
      'property info' => array(
        'domain_id' => array(
          'type' => 'domain_id',
          'label' => t('Domain Id'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'subdomain' => array(
          'type' => 'text',
          'label' => t('Domain'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'sitename' => array(
          'type' => 'text',
          'label' => t('Name'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'scheme' => array(
          'type' => 'text',
          'label' => t('Domain URL scheme'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'valid' => array(
          'type' => 'boolean',
          'label' => t('Domain Status'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'weight' => array(
          'type' => 'int',
          'label' => t('Weight'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'is_default' => array(
          'type' => 'boolean',
          'label' => t('Domain Status'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'machine_name' => array(
          'type' => 'text',
          'label' => t('Machine Name'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
        'x_type' => array(
          'type' => 'text',
          'label' => t('X Type'),
          'getter callback' => 'entity_property_verbatim_get',
          'setter callback' => 'entity_property_verbatim_set',
        ),
      ),
    ),
  );
}


/**
 * Implements hook_rules_action_info()
 */
function domain_rules_context_rules_action_info() {
  $items = array();

  $items['domain_rules_context_load_domain'] = array(
    'label' => t('Load domain'),
    'parameter' => array(
      'domain_id' => array(
        'type' => 'domain_id',
        'label' => t('Domain'),
        'description' => 'The domain ID. Optional, but either of domain ID or domain name must be present',
        'optional' => TRUE,
      ),
      'subdomain' => array(
        'type' => 'text',
        'label' => t('Domain'),
        'description' => 'Domain name',
        'optional' => TRUE,
      ),
      'reset' => array(
        'type' => 'boolean',
        'label' => t('Reset'),
        'description' => 'A boolean flag to clear the static variable if necessary',
        'default value' => FALSE,
      ),
    ),
    'provides' => array(
      'domain' => array(
        'type' => 'domain',
        'label' => t('Domain'),
      ),
    ),
    'group' => t('Domain Rules Context'),
  );

  $items['domain_rules_context_get_current_domain'] = array(
    'label' => t('Get current domain'),
    'provides' => array(
      'domain' => array(
        'type' => 'domain',
        'label' => t('Domain'),
      ),
    ),
    'group' => t('Domain Rules Context'),
  );

  $items['domain_rules_context_set_domain'] = array(
    'label' => t('Set domain'),
    'parameter' => array(
      'domain_id' => array(
        'type' => 'domain_id',
        'label' => t('Domain'),
        'description' => 'Select domain to be set as active domain',
        'options list' => 'domain_rules_context_domains_options_list',
        'optional' => FALSE,
      ),
      'bootstrap' => array(
        'type' => 'boolean',
        'label' => t('Bootstrap'),
        'description' => 'Whether to run domain bootstrap load. Default FALSE.',
        'default value' => FALSE,
      ),
    ),
    'group' => t('Domain Rules Context'),
  );

  $items['domain_rules_context_reset_domain'] = array(
    'label' => t('Reset domain'),
    'parameter' => array(
      'bootstrap' => array(
        'type' => 'boolean',
        'label' => t('Bootstrap'),
        'description' => 'Whether to run domain bootstrap load. Default FALSE.',
        'default value' => FALSE,
      ),
    ),
    'group' => t('Domain Rules Context'),
  );

  return $items;
}

/**
 * Options list with domain targets.
 *
 * @return array $options_list
 */
function domain_rules_context_domains_options_list() {
  $options_list = array();

  // Add active domains.
  foreach (domain_domains() as $domain) {
    $options_list[$domain['domain_id']] = $domain['sitename'];
  }

  return $options_list;
}

function domain_rules_context_load_domain($domain_id, $subdomain, $reset) {
  return array(
    'domain' => domain_lookup($domain_id, $subdomain, $reset),
  );
}

function domain_rules_context_get_current_domain() {
  return array(
    'domain' => domain_get_domain(),
  );
}

function domain_rules_context_set_domain($domain_id, $bootstrap) {
  domain_set_domain($domain_id, $bootstrap);
}

function domain_rules_context_reset_domain($bootstrap) {
  domain_reset_domain($bootstrap);
}