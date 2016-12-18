<?php

/* выбор модуля и его экземпляра при добавлении зоны */

global $cmd;

$cmd['module_selector'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	global $user;
	if(!empty($GLOBALS['p_par']['id'])) $zone_id=$GLOBALS['p_par']['id'];
	else $zone_id=0;
	if(!empty($GLOBALS['p_par']['zone_module'])) $zone_module=$GLOBALS['p_par']['zone_module'];
	else $zone_module=0;
	$res='<select id="zone_module" name="zone_module" OnChange="JavaScript: val=this.options[this.selectedIndex].value; if(val.indexOf(\'n\')!=-1){ clear(\'addex\'); loadurlC(\'add_ex_form\',\'addex\',val.replace(\'n\',\'\'),\'new\'); show(\'addex\'); } else hide(\'addex\')">';
	if($zone_module!=0) $res.='<option value="-3">'.lng('Not defined').'</option>';
	else				 $res.='<option value="-3" selected>'.lng('Not defined').'</option>';
	if($user->super){
		if($zone_module!=-1) $res.='<option value="-1">'.lng('Admin cabinet').'</option>';
		else $res.='<option value="-1" selected>'.lng('Admin cabinet').'</option>';
	}
	if($user->super){
		if($zone_module!=-2) $res.='<option value="-2">'.lng('Update server').'</option>';
		else $res.='<option value="-2" selected>'.lng('Update server').'</option>';
	}
	$res.=module_select(0,0,$zone_module,0,0,1,1,1);
	$res.='</select>';
	if(!isset($_GET['action'])){
		$res.='<script>$(document).ready(function(){
			var select=document.getElementById("zone_module");
			for(var i=0; i<select.options.length;i++){
				if(select.options[i].value.indexOf("n")!=-1){
					select.selectedIndex=i;
					clear(\'addex\');
					var val=select.options[i].value;
					loadurlC(\'add_ex_form\',\'addex\',val.replace(\'n\',\'\'),\'new\');
					show(\'addex\');
					break;
				}
			}
		});</script>';	
	}
	$res.='<div id="addex" style="display: none;" class="addex"></div>';
	return $res;
};
$cmd['module_selector'][CMD_COMPONENT]->result=CMD_STRING;

?>