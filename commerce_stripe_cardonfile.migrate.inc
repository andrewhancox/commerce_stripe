<?php

/**
 * @file
 * Support for commerce product types.
 */

/**
 * Destination class implementing migration into commerce product types.
 */
class MigrateDestinationCommerceStripeCardOnFile extends MigrateDestinationCommerceCardOnFile {

  public function __construct() {
    parent::__construct();
  }

  /**
   * Returns a list of fields available to be mapped.
   *
   * @return array
   *   Keys: machine names of the fields (to be passed to addFieldMapping)
   *   Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    $fields                = array();
    $fields['customer_id'] = t('Stripe customer id');
    $fields['card_id']     = t('Stripe card id');

    return array_merge($fields, parent::fields());
  }

  public function import(stdClass $cardonfile, stdClass $row) {
    $cardonfile->payment_method = 'commerce_stripe|commerce_payment_commerce_stripe';
    $cardonfile->remote_id      = (string)$cardonfile->customer_id . '|' . (string)$cardonfile->card_id;

    return parent::import($cardonfile, $row);
  }

  private function get_payment_method() {
    static $paymentmethod = null;

    if ($paymentmethod == null) {
      $paymentmethod = commerce_payment_method_instance_load('commerce_stripe|commerce_payment_commerce_stripe');
    }

    return $paymentmethod;
  }

  /**
   * Import a single product type.
   *
   * @param stdClass $product_type
   *   Generic object, refilled with any fields mapped in the Migration.
   * @param stdClass $row
   *   Raw source data object - passed through to prepare/complete handlers.
   *
   * @return array|bool
   *   Array of product type machine names if successful. FALSE on failure.
   *
   * @throws MigrateException
   */
  public function savecard($card_data, $cardonfile) {
    $card            = new stdClass();
    $card->customer  = $cardonfile->customer_id;
    $card->id        = $cardonfile->card_id;
    $card->type      = $card_data->card_type;
    $card->name      = $card_data->card_name;
    $card->last4     = $card_data->card_number;
    $card->exp_month = $card_data->card_exp_month;
    $card->exp_year  = $card_data->card_exp_year;
    $card->type      = $card_data->card_type;

    _commerce_stripe_save_cardonfile($card, $card_data->uid, $this->get_payment_method(), $card_data->instance_default);
  }
}