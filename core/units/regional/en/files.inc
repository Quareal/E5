<?php

function smart_size($byte,$floor=0,$short=0){
	if($short==0){
		$k='kb';
		$m='mb';
	}
	if($short==1){
		$k='k';
		$m='m';
	}
	if($short==2){
		$k='';
		$m='';
	}
	if($byte==0) return '-';
	if($byte<1024) return '1'.$k;
	if($byte<1024*1024) return floor($byte/1024).$k;
	$res=floor($byte/1024/1024);
	$kb=floor(($byte-$res*1024*1024)/1024/100);
	if($kb>0 && !$floor) $res.='.'.$kb;
	$res.=$m;
	return $res;
}

?>