<?php

/**
 * UI for SendinBlue Attribute.
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
  );
}


/**
 * Implements hook_rules_action_info()
 */
function domain_rules_context_rules_action_info() {
  $items = array();

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
        'label' => t('Bootstarp'),
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
        'label' => t('Bootstarp'),
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

function domain_rules_context_set_domain($domain_id, $bootstrap) {
  domain_set_domain($domain_id, $bootstrap);
}

function domain_rules_context_reset_domain($bootstrap) {
  domain_reset_domain($bootstrap);
}