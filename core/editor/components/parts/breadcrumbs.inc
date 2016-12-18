<?php

/* хлебные крошки с выпадающей навигацией */

global $cmd;

$cmd['breadcrumbs'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	global $bread, $base_root;
	$result='';
	$cs_menu=0;
	if(!empty($bread)){
	    	if(count($bread)>7){
	    		$cbr=count($bread);
	    		for($i=0;$i<$cbr-7;$i++) array_shift($bread);
	    		$i=0;
	    	}
		foreach($bread AS $crumb){
			$cs_menu++;
			$elems=&$crumb->elements;
			 if(!empty($elems)){
				$result.='<div id="cmenu'.$cs_menu.'" style="display: none;">';
				$first=true;
				foreach($elems AS $elem){
					if(!$first){
						if(empty($elem->title)) $result.='<div class="elem-hr2"></div>';
						else $result.='<div class="elem-hr"></div>';
					}
					$first=false;
					if(!empty($elem->title)){
						if(!empty($elem->url)){
							//$result.='<div class="menu-elem" OnMouseOver="itemover(this);" OnClick="loc(\''.$elem->url.'\');">';
							$result.='<div class="menu-elem" OnMouseOver="itemover(this);">';
							$result.='<a href="'.$elem->url.'" class="ablack2"><div class="elem-text mini">'.$elem->title.'</div></a>';
							if(!empty($elem->conf)) $result.='<div class="elem-pic"><a href="'.$elem->conf.'"><img src="'.$base_root.'/files/editor/classic/edit.png" width="12" height="12" vspace="6" border="0"></a></div>';
							$result.='</div>';
						} else {
							$result.='<div class="menu-elem2" OnMouseOver="itemover2(this);">';
							$result.='<div class="elem-text2 mini">'.$elem->title.'</div>';
							if(!empty($elem->conf)) $result.='<div class="elem-pic"><a href="'.$elem->conf.'"><img src="'.$base_root.'/files/editor/classic/edit.png" width="12" height="12" vspace="6" border="0"></a></div>';
							$result.='</div>';
						}
					}
				}
				$result.='</div>';
			}
		}
	}	
	
	$i=0;
	if(!empty($bread)){
		foreach($bread AS $crumb){
			$i++;
			if($i>1) $result.='<img src="'.$base_root.'/files/editor/classic/arrow.png" width="12" height="9" hspace="8" align="absmiddle">';
			$result.='<nobr>';
			if(empty($crumb->url) || !empty($crumb->elements)) $result.='<span class="menu mini" OnClick="showwnd(this,'.$i.');" style="position: relative;">';
			else $result.='<a class="menu mini" href="'.$crumb->url.'" style="position: relative;">';
			$result.=$crumb->title;
			if(!empty($crumb->title2)) $result.=' «<b>'.$crumb->title2.'</b>»';
			if(empty($crumb->url)) $result.='</span>'; else $result.='</a>';
			$result.='</nobr>';
		}
	}
	
	return $result;
};
$cmd['breadcrumbs'][CMD_COMPONENT]->result=CMD_STRING;

?>