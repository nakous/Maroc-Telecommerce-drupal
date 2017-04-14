<?php
// $Id: uc_mtc.pages.inc,v 1.1.2.3 2010/03/17 12:53:24 mogtofu33 Exp $

/**
 * @file
 * mtc menu items.
 *
 */
 
 /**
 * Return mtc form to redirect mtc payment.
 */
include("utf8.php");
function uc_mtc_payment() {
return  drupal_get_form('uc_mtc_form_payment');
}
function uc_mtc_form_payment() {
 //echo 'ddddddd';
  if (!isset($_SESSION['cart_order'])) {
    drupal_set_message(t('You are not allowed to see this page, please, return to homepage.'));
    drupal_goto('cart');
  }
  $order = uc_order_load($_SESSION['cart_order']);
  //
  // if (isset($_SESSION['sespb'])) {
    // $payment_details = unserialize($_SESSION['sespb']);
    // list($type_paiement, $type_carte) = explode('|', $payment_details['uc_mtc_typepaiement']);
    // $chaine['type_paiement'] = " PBX_TYPEPAIEMENT=". $type_paiement;
    // $chaine['type_carte'] = " PBX_TYPECARTE=". $type_carte;
  // }
 // global $user;
 // echo $user->mail;

// print_r($order);
$dataMD5=variable_get('actionSLK', '') . variable_get('storeId', '') . $order->order_id . $order->order_total . $user->mail . variable_get('SLKSecretKey', '');
// echo $dataMD5;
$checksum=MD5(utf8entities(rawurlencode($dataMD5)));
drupal_add_js("\$(document).ready(function() { \$(' #edit-submit').click(); });", 'inline');
	$form['#name'] = "paymentForm";
	$form['#action'] = variable_get('actionSLK', '');
	$form['#method'] = 'post';
	$form['storeId'] = array('#type' => 'hidden', '#value' =>variable_get('storeId', '')) ;
	$form['langue'] = array('#type' => 'hidden', '#value' =>"FR") ;
	$form['offerURL'] = array('#type' => 'hidden', '#value' =>_uc_mtc_url()) ;
	$form['updateURL'] = array('#type' => 'hidden', '#value' =>_uc_mtc_url().'cart/mtc/effectue') ;
	$form['bookURL'] = array('#type' => 'hidden', '#value' =>_uc_mtc_url().'cart/mtc/update') ;
	$form['cartId'] = array('#type' => 'hidden', '#value' =>$order->order_id) ;
	$form['totalAmountTx'] = array('#type' => 'hidden', '#value' =>$order->order_total) ;
	// $form['totalAmountCur'] = array('#type' => 'hidden', '#value' =>'200') ;
	$form['symbolCur'] = array('#type' => 'hidden', '#value' =>'MAD') ;
	$name =$order->delivery_first_name.' '.$order->delivery_last_name;
	$form['name'] = array('#type' => 'hidden', '#value' =>$name) ;
	$adresse=$order->delivery_street1.' '.$order->delivery_street2;
	$form['address'] = array('#type' => 'hidden', '#value' =>$adresse) ;
	$form['city'] = array('#type' => 'hidden', '#value' =>$order->delivery_city) ;
	$form['state'] = array('#type' => 'hidden', '#value' =>uc_zone_get_by_id($order->delivery_zone)) ;
	$form['country'] = array('#type' => 'hidden', '#value' => 'MA') ;
	$form['postCode'] = array('#type' => 'hidden', '#value' => $order->delivery_postal_code) ;
	$form['tel'] = array('#type' => 'hidden', '#value' =>$order->delivery_phone) ;
	$form['email'] = array('#type' => 'hidden', '#value' => 'ddddddd');//$user->mail) ;
	$form['checksum'] = array('#type' => 'hidden', '#value' =>$checksum) ;
	$form['paiement'] = array('#type' => 'hidden', '#value' =>'Payer') ;
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Continue checkout'),
	);


  if ($form) {
   return $form;
  }
  else {
    drupal_set_message(t('An error has occured during payment, please contact us.'), 'error');
    watchdog('uc_mtc', 'CGI error: order @orderid, path: @path', array('@orderid' => $orderid, '@path' => $path_bin), WATCHDOG_WARNING);
    drupal_goto('cart');
  }
}

 /**
 * Customer return after mtc validation
 */
function uc_mtc_effectue() {

$updateURL=_uc_mtc_url().'cart/mtc/effectue';
$SLKSecretkey=variable_get('SLKSecretKey', '') ;
$storeId = variable_get('storeId', '');
$cartId=$_GET["cartId"];
$checksumMTC=$_GET["checksum"];
$totalAmountTx=$_GET["totalAmountTx"];
//$email=<récupéré à partir de la BDD ou les variables de sessions du site marchand"
//$storeId=<valeur qui a été renseignée par Maroc Telecommerce>
$storeId = variable_get('storeId', '');
global $user;
$email=$user->mail;
$dataMD5=$updateURL . $storeId . $cartId . $totalAmountTx . $email . $SLKSecretkey;

$checksum=MD5(utf8entities(rawurlencode($dataMD5)));

if ($checksum == $checksumMTC) {
//D'abord il faut vérifier est-ce que le paiement a bien été confirmé via la requête bookURL. Si c'est le cas, afficher un message de confirmation de paiement et de prise en compte de la commande
        $order = uc_order_load($cartId);
		// print variable_get('uc_new_customer_login', FALSE);
      // process order (without payment, status set to Pending -> only auto can validate)
      $output = uc_cart_complete_sale($order);
      // add comment and log
	   // print_r( $order);
	   
	  $ordernumber=db_result(db_query("SELECT nbpay FROM {uc_order_number} where orderid=%d ",$cartId));
	  
      uc_order_comment_save($cartId, $user->uid, t('Customer get back on successful payment, waiting auto notification to validate payment.'), 'order');
      watchdog('uc_mtc', 'Payment back on order !orderid, waiting auto notification.', array('!orderid' => $cartId));
      // check if alternate checkout completion page is set
	  drupal_set_message('Votre commande a bien &eacute;t&eacute; accept&eacute;e sous la r&eacute;f&eacute;rence N : '.$cartId .' et le paiement N : '.$ordernumber);
      $page = variable_get('uc_cart_checkout_complete_page', '');
      if (!empty($page)) {
       drupal_goto($page);
      }
      return $output;
} else {
         drupal_set_message(t('An error has occured during payment, please contact us.') . t('Error: @error', array('@error' => $error)), 'error');
      // add comment and log
      uc_order_comment_save($orderid, $user->uid, t('Error occured during payment: @error', array('@error' => $error)), 'order');
      watchdog('uc_mtc', 'Error on complete : @error', array('@error' => $error), WATCHDOG_WARNING);
      drupal_goto('cart');
} 
  // debug
  // if (variable_get('uc_mtc_debug', FALSE) == TRUE) {
    // foreach ($_GET as $k => $v) {
      // drupal_set_message($k .': '. $v);
    // }
   // }
  // else {
    // drupal_set_message(t('An error has occured during payment, please contact us.'), 'error');
    // watchdog('uc_mtc', 'Sign error: order @orderid', array('@orderid' => $orderid), WATCHDOG_WARNING);
    // drupal_goto('cart');
  // }
}
function uc_mtc_update(){
// $dataMD5= .  . $order->order_id . $order->order_total . $email . ;variable_get('actionSLK', '')
$bookURL=_uc_mtc_url().'cart/mtc/update';
$SLKSecretkey= variable_get('SLKSecretKey', '') ;
$storeId = variable_get('storeId', '');
$cartId=$_POST["cartId"];
$email=$_POST["email"];
$totalAmountTx=$_POST["totalAmountTx"];
$checksumMTC=$_POST["checksum"];
$orderNumber=$_POST["orderNumber"];

//parametre de cart
$bankName=$_POST["bankName"];
$expirationDate=$_POST["expirationDate"];
$ccn=$_POST["ccn"];
$paymentBrandName=$_POST["paymentBrandName"];

$dataMD5=$bookURL . $storeId . $cartId . $totalAmountTx . $email . $SLKSecretkey;

$checksum=MD5(utf8entities(rawurlencode($dataMD5)));

$resultat=db_result(db_query('select count(*) from badget where bn=%s and ccn=%d and pb=%s',$bankName,$ccn,$paymentBrandName));
$countcart=db_result(db_query('select count(*) from badget where buid=%s ',$user->uid));

if($resultat==0 and $countcart<4){
		db_query("INSERT INTO {badget} (`bn`, `dx`, `ccn`, `pb`, `buid`, `count`) VALUES ('%s', '%s', '%s', '%s', '%d', '%d')",$bankName,$expirationDate,$ccn,$paymentBrandName,$user->uid,1);
	}else{
		$cart=db_fetch_array(db_query('select * from badget where bn=%s and ccn=%d and pb=%s',$bankName,$ccn,$paymentBrandName));
		if($cart['buid']!=$user->uid ){ echo "0;Null;Null;1";exit;}
}


if ($checksum == $checksumMTC && is_numeric($orderNumber) == "True") {
//  "Mettre à jour la base de données du site marchand en vérifiant si la commande existe et correspond au retour MTC!"
//  "Dans cette MAJ, il faut enregistrer le n° du Bon de commande de paiement envoyé dans le paramètre ""orderNumber"" "
   echo "1;" . $cartId . ";".date("Ymd").";2";
   db_query("INSERT INTO {uc_order_number} (`orderid`, `nbpay`) VALUES ('%d', '%d')",$cartId,$orderNumber);
} else {
//  "Rejeter la demande !"
   echo "0;Null;Null;1";
} 
}
 /**
 * Customer return after mtc refuse payment
 */
function uc_mtc_refuse() {
  drupal_set_message(t('Your purchase has been refused. Please try again or contact us.'));
  drupal_goto('cart');
}

 /**
 * Customer return after mtc cancel payment
 */
function uc_mtc_annule() {
  $orderid = $_GET['orderid'];
  $error = FALSE;

  $order = uc_order_load($orderid);
  if ($order) {
    unset($_SESSION['cart_order']);
    // add comment
    uc_order_comment_save($orderid, $user->uid, t('This order has been explicitly canceled by the user.'), 'order');
    // check status
    if (uc_order_update_status($orderid, uc_order_state_default('canceled'))) {
      drupal_set_message(t('Your order has been canceled.'), 'status');
    }
    else {
      $error = TRUE;
      watchdog('uc_mtc', "Order @order couldn't be marked as 'Canceled'.", array('@order' => $orderid), WATCHDOG_WARNING, NULL);
    }
  }
  else {
    $error = TRUE;
    watchdog('uc_mtc', 'Order cancel error: order @orderid', array('@orderid' => $orderid), WATCHDOG_WARNING);
  }
  if ($error) {
    drupal_set_message(t('An error has occured during payment, please contact us.'), 'error');
  }
  drupal_goto('cart');
}

 /**
 * auto notification from mtc
 */
function uc_mtc_auto() {

  $orderid = $_GET['orderid'];
  $montant = $_GET['montant'];
  $uniqueid = $_GET['uniqueid'];
  $erreur = $_GET['erreur'];
  $sign = $_GET['sign'];
  $ip_mtc = ip_address();

  // check ip
  if (!_uc_mtc_ip_check($ip_mtc)) {
    $errors[] = t('Invalid IP @ip.', array('@ip' => $ip_mtc));
  }
  // check sign
  if (!_uc_mtc_verif_sign()) {
    $errors[] = t('Invalid sign.');
  }

  // check order
  $order = uc_order_load($orderid);
  if (!$order) {
    $errors[] = t('Invalid order @id.', array('@id' => $order->order_id));
  }
  else {
    // Check status
    if ($order->order_status != 'in_checkout' && $order->order_status != 'pending') {
      $errors[] = t('Order not in checkout or pending: ') . $order->order_status;
    }
    if ($order->order_status == 'payment_received') {
      $errors[] = t('Try to revalide a payment.');
    }
    // Check price
    $total = number_format($order->order_total, 2) * 100;
    if ($total != $montant) {
      $errors[] = t('Incorrect price: @price1 / @price2.', array('@price1' => $total, '@price2' => $montant));
    }
    // Check error code
    if ($erreur != "00000") {
      $errors[] = _uc_mtc_erreur($erreur);
    }
  }
  if (!empty($errors)) {
    // Log errors to order and watchdog.
    $error_data = array('@id' => $orderid, '@error' => implode(" - ", $errors));
    uc_order_comment_save($order->order_id, 0, t('Autoresponse error on order @id: @error', $error_data), 'admin');
    watchdog('uc_mtc', 'Order @id caused error: @error', $error_data, WATCHDOG_ERROR);
    // debug
    if (variable_get('uc_mtc_debug', FALSE) == TRUE) {
      $output = t('Order @id caused error: @error', $error_data);
      print $output;
    }
    exit();
  }

  // if everything ok, process
  // Sale complete
  uc_order_update_status($order->order_id, 'payment_received');
  $comment = t('Paid by mtc reference @uniqueid.', array('@uniqueid' => $uniqueid));
  uc_payment_enter($order->order_id, 'uc_mtc', $montant / 100, 0, NULL, $comment);
  uc_order_comment_save($order->order_id, 0, t('Order created through website. mtc payment transaction : @uniqueid', array('@uniqueid' => $uniqueid)), 'admin');
  uc_order_comment_save($order->order_id, 0, t('Payment accepted. mtc transaction id: @uniqueid', array('@uniqueid' => $uniqueid)), 'order');
  // debug
  if (variable_get('uc_mtc_debug', FALSE) == TRUE) {
    print $comment;
  }
  exit();
}

/**
 * Define mtc error status, see mtc doc
 */
function _uc_mtc_erreur($erreur) {
  switch ($erreur) {
    case "00001":
      $output['infos'] = t('Connexion to autorise center failed.');
      break;
    case substr($erreur, 0, 3) == "001":
      $output['infos'] = t('Payment refused by autorisation center.');
      break;
    case "00003":
      $output['infos'] = t('mtc error.');
      break;
    case "00004":
      $output['infos'] = t('Owner number or cryptogram invalid.');
      break;
    case "00006":
      $output['infos'] = t('Access refused or rank/site/is incorrect.');
      break;
    case "00008":
      $output['infos'] = t('Error on expiry date');
      break;
    case "00009":
      $output['infos'] = t('Error creating subscription.');
      break;
    case "00010":
      $output['infos'] = t('Unknow currency.');
      break;
    case "00011":
      $output['infos'] = t('Wrong order total.');
      break;
    case "00015":
      $output['infos'] = t('Payment already validate.');
      break;
    case "00016":
      $output['infos'] = t('User exist.');
      break;
    case "00021":
      $output['infos'] = t('Unauthorize card.');
      break;
    case "00029":
      $output['infos'] = t('Card non-compliant.');
      break;
    case "00030":
      $output['infos'] = t('Timeout on checkout page (> 15 mn).');
      break;
    case "00031":
      $output['infos'] = t('Reserved.');
      break;
    default:
      $output['infos'] = t('Unknown error');
  }
  return t('ERROR @error: ', array('@error' => $erreur)) . $output['infos'];
}

/**
 * Check sign on mtc back url
 */
function _uc_mtc_verif_sign() {
  $q = preg_replace("/q=[^&]+&/", "", $_SERVER['QUERY_STRING']);
  $pos = strrpos($q, '&');
  $data = substr($q, 0, $pos);
  $pos = strpos($q, '=', $pos) + 1;
  $sig = substr($q, $pos);
  $sig = base64_decode(urldecode($sig));
  $key = openssl_pkey_get_public(file_get_contents(drupal_get_path('module', 'uc_mtc') .'/pubkey.pem'));
  return openssl_verify($data, $sig, $key);
}
