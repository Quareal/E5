<?php

/* вывод последних новостей пользователя */

global $cmd;

$cmd['news'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	$result='';
	global $sel,$ppz,$zone,$zone_url,$menu,$user,$db,$base_root,$cz,$module;
	if($module!='news'){
		if(isset($_GET["nid"])) $nn=get_news(3,0,-1,-2,-2,0,0,Array($_GET["nid"])); else $nn=get_news(3);
		if(!empty($nn)){
			global $fullurl2;
			$result.='<div class="lblock"><a href="'.$zone_url.'/news"><img src="'.$base_root.'/files/editor/classic/news.png" border="0" align="absmiddle" style="margin-right: 5px;"></a> <b style="color: #565656;">Новости</b>';
			$i=0;
			foreach($nn AS $n){
				$i++;
				$result.='<table width="175" cellpadding="0" cellspacing="0" style="margin-left: 6px;"><tr><td valign="top" width="14" align="left"'.($i!=count($nn)?' class="lb_bg"':'').($i==1?'style="padding-top: 5px;"':'').'><img src="/files/editor/classic/lb1_corner.png"></td><td'.($i==1?' style="padding-top: 5px;"':'').'><div style="padding-bottom: 5px;">';
				$u=$zone_url.'/news#id'.$n->id;
				if($n->url!='') $u=$n->url;
				if(strstr($u,'mod_table')){
					$u=explode('#',$u);
					$u=$zone_url.'/'.$u[0].'&amp;nid='.$n->id.'#'.$u[1];
				}
				if(strpos($fullurl2,str_replace('&amp;','&',$u))!==false) $result.='<b class="normal">'.crop_longtext(clean_html($n->title),128).'</b>';
				else $result.='<a href="'.$u.'" class="agray">'.crop_longtext(clean_html($n->title),128).'</a>';
				$result.='</div></td></tr></table>';
			}
			$result.='<div align="center" style="padding-top: 7px; padding-bottom: 7px;" class="normal"><a href="'.$zone_url.'/news" class="ablue">Все новости</a></div>';
			$result.='</div>';
		}
	}
	return $result;
};
$cmd['news'][CMD_COMPONENT]->result=CMD_STRING;

?>