<?php

/* вывод блока с выбором сайта и его модулей */

function get_blocks(){
	global $cz,$zone,$ncz,$xurl,$db,$module,$id2,$id,$ncz,$gd,$sel,$stm,$ddm,$zone_url,$m,$m2,$base_root,$zone_url;
	if(isset($GLOBALS["call_get_blocks"])) return false;
	$GLOBALS["call_get_blocks"]=1;
	if(isset($_POST["id2"])) $id2=$_POST["id2"];
	if(strstr($_SERVER["REQUEST_URI"],'/mod_table') && !empty($id2)){
		$znc=getall4($db,"SELECT * FROM ex_zone","ex_module");
		$czs=getall($db,"SELECT * FROM ex_group WHERE ex_ex1=$id2",1,"ex_group");
		if(!empty($czs) && count($czs)==1){
			foreach($czs AS $tmp){ $czs=$tmp; break; }
			if(!empty($znc[$czs["ex_ex2"]])){
				$cz=$znc[$czs["ex_ex2"]]["ex_zone"];
			}
		}
	}
	
	if(!empty($ncz)) $cz=$ncz;
	if($xurl==''){
		getrow($db,"SELECT * FROM main_zone WHERE zone_domain='".$_SERVER["HTTP_HOST2"]."' AND zone_active=1",1,"main_zone");
		while(!empty($db->Record) && $db->Record["zone_redirect"]!=0){
			getrow($db,"SELECT * FROM main_zone WHERE zone_id=".$db->Record["zone_redirect"]." AND zone_active=1",1,"main_zone");
		}
		if(!empty($db->Record)) $cz=$db->Record["zone_id"];
	}
	/*if(empty($m)) */$m=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,"main_module");
	$m2=Array();
	if(!empty($m)) foreach($m AS $mm){
		$m2[$mm["module_id"]]=$mm;
	}
	$ezns=getall($db,"SELECT * FROM ex_zone",1,"ex_zone");$ez=Array();
	if(!empty($ezns)) foreach($ezns AS $ezn) if(check_ex($ezn["ex_module"],'view')){
		$ez[$ezn["ex_zone"]][$ezn["ex_module2"]][]=$ezn;
	}

	$tmp=getall($db,"SELECT * FROM main_table WHERE table_bold=1",1,"main_table");$bt=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) if(check_tbl($tm["table_id"],'view') && check_mod($tm["table_module"],'view')) $bt[$tm["table_module"]]=$tm["table_id"];
	$tmp=getall($db,"SELECT * FROM ex_group",1,"ex_group");$elink=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) $elink[$tm["ex_table"]][$tm["ex_ex2"]]=$tm["ex_ex1"];

	$tmp=getall($db,"SELECT * FROM ex_module",1,"ex_module");$exm=Array();
	if(!empty($tmp)) foreach($tmp AS $tm){
		if($tm["ex_major"]!=0) $ddm[$tm["ex_id"]]=$tm["ex_major"];
		$exm[$tm["ex_id"]]=$tm["ex_name"];
	}

	$sel=Array();
	$gd=Array();
	if(!empty($zone)) foreach($zone AS $ppz) if($ppz["zone_active"]){
		$add='none';
		if($cz==$ppz["zone_id"]) $add='';
		$first=true;
		$gd[$ppz["zone_id"]]->name=$ppz["zone_name"];
		//ниже строчка опупеть
		$tprt=0; $cprt=0;
		if(!empty($m)) foreach($m AS $cm) if(isset($ez[$ppz["zone_id"]][$cm["module_id"]]))foreach($ez[$ppz["zone_id"]][$cm["module_id"]] AS $ez2) if(isset($bt[$cm["module_id"]]) && isset($elink[$bt[$cm["module_id"]]]) && isset($elink[$bt[$cm["module_id"]]][$ez2["ex_module"]])) $tprt++;
		if(!empty($m)) foreach($m AS $cm) if(isset($ez[$ppz["zone_id"]][$cm["module_id"]]))foreach($ez[$ppz["zone_id"]][$cm["module_id"]] AS $ez2) if(isset($bt[$cm["module_id"]]) && isset($elink[$bt[$cm["module_id"]]]) && isset($elink[$bt[$cm["module_id"]]][$ez2["ex_module"]]))			
		/*if(check_mod($cm["module_id"],"view") && check_ex($ez2["ex_module"],'view'))*/{
			$cprt++;
			if($first){
				if(empty($cz)){
					$add=' selected';
					$cz=$ppz["zone_id"];
				}
				$first=false;
				$sel[$ppz["zone_id"]]='<div style="display:'.$add.';" id="pz'.$ppz["zone_id"].'"><table width="175" cellpadding="0" cellspacing="0" style="margin-left: 6px;">';
			}
			// и тут тоже этот гениальный супер массив. интересно сколько ресурсов он жрёт			
			$stm[$cm["module_id"]]=1;
			
			$sel[$ppz["zone_id"]].='<tr><td valign="top" width="14" align="left"';
			if(!($cprt==$tprt)) $sel[$ppz["zone_id"]].=' class="lb_bg"';
			$sel[$ppz["zone_id"]].='><img src="'.$base_root.'/files/editor/classic/lb1_corner.png"></td><td><div style="padding-bottom: 3px;">';			
			$is_link=true;
			$tex=$elink[$bt[$cm["module_id"]]][$ez2["ex_module"]];
			if(!empty($ddm[$ez2["ex_module"]])) $tex=$elink[$ddm[$ez2["ex_module"]]][$ez2["ex_module"]];			
			if($module=='mod_table' && !empty($id2) && $id==$cm["module_id"] && $id2==$tex) $is_link=false;
			$t_url=$GLOBALS["zone_url2"].'/mod_table?id='.$cm["module_id"].'&amp;id2='.$tex.'&amp;action=setcz&amp;ncz='.$ppz["zone_id"].'&amp;ncm='.$ez2["ex_module"];
			$t_url2=$zone_url.'/mod_main?id='.$cm["module_id"].'&amp;id2='.$ez2["ex_module"].'&amp;action=edit_ex_form#edit_ex';
			if(!check_ex($ez2["ex_module"],'edit',$cm["module_id"])) $t_url2='';
			$t_name=$cm["module_name"];
			if(!empty($exm[$ez2["ex_module"]])) $title2=$exm[$ez2["ex_module"]]; else $title2='';
			if($is_link){
				$sel[$ppz["zone_id"]].='<a href="'.$t_url.'" class="ablack normal">';
				$sel[$ppz["zone_id"]].=$cm["module_name"];
				if(count($ez[$ppz["zone_id"]][$cm["module_id"]])>1){
					$t_name.=' <span style="font-size: 11px; color: #555555;">('.$exm[$ez2["ex_module"]].')</span>';
				} else $title2='';				
				$sel[$ppz["zone_id"]].='</a> ';
				if(!empty($t_url2)) $sel[$ppz["zone_id"]].='<a href="'.$t_url2.'"><img src="'.$base_root.'/files/editor/classic/lb1_cfg.png" border="0" align="absmiddle"></a>';
				if($title2!='') $sel[$ppz["zone_id"]].='<div class="gray">(<a href="'.$t_url.'" class="agray">'.$title2.'</a>)</div>';
			} else {
				$sel[$ppz["zone_id"]].='<b class="normal">'.$cm["module_name"].'</b> ';
				if(!empty($t_url2)) $sel[$ppz["zone_id"]].='<a href="'.$t_url2.'"><img src="'.$base_root.'/files/editor/classic/lb1_cfg.png" border="0" align="absmiddle"></a>';
				if($title2!='') $sel[$ppz["zone_id"]].='<div class="gray">('.$title2.')</div>';	
			}
			$gd[$ppz["zone_id"]]->comp[$t_url]=$t_name;
			$sel[$ppz["zone_id"]].='</div></td></tr>';
		}
		$thost=$ppz["zone_domain"];
		$thost2='http://'.$ppz["zone_domain"];
		if($ppz["zone_domain"]==''){ $thost='/'; $thost2='';}
		if(!$first) $sel[$ppz["zone_id"]].='</table><div align="center" style="padding-top: 10px; padding-bottom: 7px;" class="normal"><a href="'.$thost2.'/'.$ppz["zone_folder"].'" class="ablue">'.$thost.($ppz["zone_folder"]?($thost!='/'?'/':'').$ppz["zone_folder"]:'').'</a></div></div>';
		$gd[$ppz["zone_id"]]->url=$thost2.'/'.$ppz["zone_folder"];
	}
}

global $cmd;

$cmd['zone_select'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	$result='';
	global $sel,$ppz,$zone,$zone_url,$menu,$user,$db,$base_root,$cz;	
	get_blocks();
	if(!empty($sel) && count($sel)>0){
		$result.='<script>var lobj2="pz"+'.$cz.';
		function upd2(obj2){
		';
		$result.='if(lobj2!=0){
			var o=document.getElementById(lobj2);
			if(o!=null) document.getElementById(lobj2).style.display="none";
		}
		var obj=document.getElementById("pz"+obj2.options[obj2.selectedIndex].value);
		obj.style.display="";
		lobj2="pz"+obj2.options[obj2.selectedIndex].value;';
		$result.='}</script>';
		$result.='<div class="lblock">
			<div><a href="'.$zone_url.'/zones"><img src="'.$base_root.'/files/editor/classic/zone.png" border="0" align="absmiddle"></a><select name="zs" id="zs" class="zone_select" OnChange="upd2(this)">';
		$ccz=0;
		//подсчёт кол-ва активных зон, нужно для определения не один ли там элемент
		if(!empty($zone)) foreach($zone AS $ppz)if(!empty($sel[$ppz["zone_id"]])){
			$ccz++;
		}
		if(!empty($zone)) foreach($zone AS $ppz)if(!empty($sel[$ppz["zone_id"]])){
			$add='';
			if($cz==$ppz["zone_id"] || $ccz==1) $add=' selected';
			$result.='<option value="'.$ppz["zone_id"].'"'.$add.'>'.$ppz["zone_name"].'</option>';
		}
		$result.='</select></div>';
		
		foreach($sel AS $vs){
			if($ccz==1) $vs=str_replace('display:none;','',$vs);
			$result.=$vs;
		}
		$result.='</div>';
	}
	return $result;
};
$cmd['zone_select'][CMD_COMPONENT]->result=CMD_STRING;

?>