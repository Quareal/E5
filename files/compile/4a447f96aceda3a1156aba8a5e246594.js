
        function submit_form(id,ajax,url){
            var f='form_check'+id;
            var fe='gain_error'+id;
            var error=window[f]();
            if(error!='' && error!=false){
                window[fe](error);
                return false;
            } else {
                if(ajax){
                    jQuery.ajax({
                        url:      url,
                        type:     "POST",
                        dataType: "html",
                        data: jQuery("#form"+id).serialize(),
                        success: function(response) {
                            $('#form'+id).hide();
                            $('#answer_body'+id).show();
                            $('#answer_body'+id).html(response);
                        }
                    });
                    return false;
                } else return true;
            }
        }
    