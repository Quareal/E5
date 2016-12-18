<?php

$auto_update=false;//Автомотически обновляет тумбы каждый месяц
global $user;

if(!empty($_GET["refresh"]) && $user->super){
	$z=urldecode($_GET["refresh"]);
	if($auto_update) $uu=md5($z.date('m')); else $uu=md5($z);
	if(file_exists(DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'.png')) unlink(DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'.png');
	if(file_exists(DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'fi.png')) unlink(DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'fi.png');
}

echo '<br><div style="width: 85%;">';
$i=0;
check_dir('/files/editor/preview');
if(!isset($gd)){
	include_once(DOCUMENT_ROOT.'/core/editor/components/parts/zone_select.inc');
	get_blocks();
}
$f=false;
if(!empty($gd) && count($gd)>0){
	foreach($gd AS $zid=>$g) if(!empty($g->comp)){
		$f=true;
		if($auto_update) $uu=md5($g->url.date('m')); else $uu=md5($g->url);
		if(strstr($g->url,'http://')) $bgu='background-image: url('.$GLOBALS["base_root"].'/files/editor/preview/'.$uu.'.png);background-position: top left; background-repeat: no-repeat;'; else $bgu='';
		$fi='<img src="'.$GLOBALS["base_root"].'/files/editor/site.png" width=16 height=16 border="0" style="margin-right: 5px;" align="absmiddle">';
		if(strstr($g->url,'http://') && file_exists(DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'fi.png') && filesize(DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'fi.png')>0) $fi='<img height="16" src="'.$GLOBALS["base_root"].'/files/editor/preview/'.$uu.'fi.png" border="0" style="margin-right: 5px;" align="absmiddle">';
		echo '<div style="width: 220px; float: left; clear: right; padding: 5px;">';
		echo '<div style="padding: 5px; background-color: #1076DC; color: #FFFFFF;"><b><a href="'.$g->url.'" target="_blank" style="color: #FFFFFF;">'.$fi.$g->name.'</a></b>';
		if($user->super) echo '<a href="?refresh='.urlencode($g->url).'"><img src="'.$GLOBALS["base_root"].'/files/editor/refresh.png" height=16 align="right" border=0></a>';
		echo '</div>';
		echo '<div style="border-left: 1px solid #1076DC; border-right: 1px solid #1076DC; border-bottom: 1px solid #1076DC;padding: 5px;'.$bgu.'"><div style="min-height: 150px;">';
		if(!empty($g->comp)) foreach($g->comp AS $url=>$tit){
			echo '<div style="background-image: url('.$GLOBALS["base_root"].'/files/editor/wb.png); padding-left: 7px; padding-top: 5px; padding-bottom: 5px; padding-right: 10px; font-size: 12px;"><img src="'.$GLOBALS["base_root"].'/files/editor/li2.png" align="absmiddle" style="margin-right: 3px;"> <a href="'.$url.'" class="black">'.$tit.'</a></div>';
		}
		echo '</div></div>';
		echo '</div>';
	}
}
if(!$f) include('modules.php');

if(check_zone(0,'add')) echo '<div style="width: 220px; float: left; clear: right; padding: 5px; margin-top: 25px;" align="center">
	<a href="'.$zone_url.'/zones#add_form" class="ablack big"><img src="'.$base_root.'/files/editor/addbig.png" border="0"><div style="margin-top: 10px;">Добавить сайт</div></a>
	</div>';


echo '</div>';

function is_pic($filename){
  $is = @getimagesize($filename);
  $tmp=file_get_contents($filename);
  if(substr5($tmp,0,3)=='BM8') return true;
  if (!$is) return false;
  elseif ( !in_array($is[2], array(1,2,3)) ) return false;
  else return true;
}

$cnt=0;$max=2;//кол-во миниатюр, загружаемых за 1 раз
if(!empty($gd) && count($gd)>0){
	$date=explode('-',date('Y-m-d'));
	$a=mktime(0,0,0,$date[1]-1,$date[2],$date[0]);
	$old=date('m', $a);
	$fst=true;
	foreach($gd AS $zid=>$g) if(!empty($g->comp)){
		if(strstr($g->url,'http://')){
			if($auto_update) $uu=md5($g->url.date('m')); else $uu=md5($g->url);
			$bgu=DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'.png';
			$fi=DOCUMENT_ROOT.'/files/editor/preview/'.$uu.'fi.png';
			if(!file_exists($fi)){
				if($auto_update){
					$old2=DOCUMENT_ROOT.'/files/editor/preview/'.md5($g->url.$old).'fi.png';
					if(file_exists($old2))unlink($old2);
				}
				$t=@file_get_contents($g->url);
				if(empty($t)){
					$f=fopen($fi,'w+');fclose($f);
				} else {
					$t=get_tag(strtolower($t),'<link rel="shortcut icon" href="','"');
					if(empty($t)){
						@$t=file_get_contents($g->url.'favicon.ico');
						if(!empty($t) && is_pic($g->url.'favicon.ico')){
							$f=fopen($fi,'w+');fwrite($f,$t);fclose($f);
						} else {
							$f=fopen($fi,'w+');fclose($f);
						}
					} else {
						if($t[0]=='/') $rur=substr5($g->url,0,strlen5($g->url)-1).$t;
						else if(substr5($t,0,4)=='http') $rur=$t;
						else $rur=$g->url.$t;								
						if(!@copy($rur,$fi) || !is_pic($rur)){
							$f=fopen($fi,'w+');fclose($f);
						}
					}
				}
			}
			if(!file_exists($bgu) && $cnt<$max){
				$cnt++;
				if($fst){
					echo '<div align="right" id="loading">Загружаются миниатюры. Пожалуйста подождите.</div>';
					ignore_user_abort(true);
					set_time_limit(0);
					ob_end_flush();
				}
				$fst=false;
				if($auto_update){
					$old2=DOCUMENT_ROOT.'/files/editor/preview/'.md5($g->url.$old).'.png';
					if(file_exists($old2))unlink($old2);
				}
				@copy('http://mini.s-shot.ru/800x1000/220/JPEG/?'.str_replace('http://','',$g->url),$bgu);
				/*$v["data[Screenshot][url]"]=$g->url;
				$v["data[Screenshot][width]"]=210;
				$v["width"]=210;
				$v["quality"]=2;
				$v["data[Screenshot][quality]"]=2;
				@$tmp=do_post('http://www.browsrcamp.com/app/screenshots',$v);
				if(!empty($tmp)){
					$d=get_tag($tmp,' target="blank"><img src="','"');
					@copy($d,$bgu);
				} else {					
					$f=fopen($bgu,'w+');fclose($f);
				}*/
			}
		}
	}
}
if(isset($fst) && !$fst){
	echo '<script>document.getElementById("loading").style.display="none";</script>';
}

?>