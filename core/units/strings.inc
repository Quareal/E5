<?php

if(!empty($local_region) && file_exists(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/strings.inc')) include_once(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/strings.inc');
if(!empty($local_region) && file_exists(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/search.inc')) include_once(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/search.inc');

function clear_dblspace($str){
	if(is_object($str) || is_array($str)) return $str;
	$str=str_replace('
',' ',$str);
	$str=str_replace(VSP,' ',$str);
	$str=str_replace(VSP2,' ',$str);
	$str=str_replace('	',' ',$str);
	while(strstr5($str,'  ')) $str=str_replace('  ',' ',$str);
	return $str;
}

function multy_strstr($dst,$search=Array(),$lowerstr=0,$only_postfix=0){//lowerstr - преобразовывать в нижний регистр, only_postfix - смотреть только окончания
	if($lowerstr) $dst=strtolower($dst);
	foreach($search AS $s){
		if(!$only_postfix && strstr($dst,$s)) return true;
		if($only_postfix && substr($dst,0,strlen($s))==$s) return true;
	}
	return false;
}

function seek_baseroot($f){
	$sdr=$_SERVER["DOCUMENT_ROOT"];
	//if(strlen($f)>4 && strlen($sdr)>4 && substr($sdr,0,4)!=substr($f,0,4)) return '';
	if(strlen($f)>=strlen($sdr)){
		$max=$f;
		$min=$sdr;
	} else {
		$max=$sdr;
		$min=$f;
	}
	$sdr=strlen($max);
	$f=str_replace("\\","/",$f);//для WIN
	$min=str_replace("\\","/",$min);//для WIN
	$max=str_replace("\\","/",$max);//для WIN
	$base_root='';
	for($i=0;$i<$sdr;$i++) if(!isset($min[$i]) || $max[$i]!=$min[$i]){
		$base_root=substr($f,$i,strlen($f)-$i-10);
		if(!empty($base_root) && $base_root[0]!='/') $base_root='/'.$base_root;
		break;
	}
	if($base_root=='/index.ph') $base_root='';
	return $base_root;
}

function trim_all($str){
	$str=trim($str);
	$spaces=chr(32).chr(9).chr(10).chr(13);
	while(!empty($str) && (strstr($spaces,$str[0]) || strstr($spaces,$str[strlen($str)-1]))) $str=trim($str);
	return $str;
}

function bstrstr($v1,$v2){
	for($i=0;$i<strlen5($v1);$i++) if(empty($v2[$i]) || $v1[$i]!=$v2[$i]) return false;
	return true;
}

function substr_unicode($str, $s, $l = null) {
     return join("", array_slice(
         preg_split("//u", $str, -1, PREG_SPLIT_NO_EMPTY), $s, $l));
 }
 
function in_ext($src,$exts=Array()){
	$src=strtolower($src);
	$src=explode('.',$src);
	if(empty($src[1])) return false;
	foreach($exts AS $ext) if($src[1]==$ext) return true;
	return false;
}

function bstrstr2($v1,$v2){
	if(strlen5($v1)>2 && $v1[1]=='*'){	
		$x=strpos($v2,'/'.substr5($v1,2));
		if($x) $GLOBALS["zone_prefix"]=substr5($v2,1,$x-1);
		return $x!==false;
	}
	for($i=0;$i<strlen5($v1);$i++) if(empty($v2[$i]) || $v1[$i]!=$v2[$i]) return false;
	return true;
	
	/*echo '<br>';
	for($i=strlen5($v1)-1;$i>=0;$i--){
		echo $v1[$i];
		if($v1[$i]=='*') return true;
		if(empty($v2[$i]) || ($v1[$i]!=$v2[$i] && $v1[$i]!='*')) return false;
	}
	return true;*/
}

function str_replace1($s1,$s2,$s3){
	$in1=strpos5($s3,$s1);
	return substr_replace($s3,$s2,$in1,strlen5($s1));
}

function enshurl($str){
	$xstr='';
	if(substr($str,0,7)=='http://' || substr($str,0,8)=='https://'){
		//if($str[strlen($str)-1]=='/') $str=substr($str,0,strlen($str)-1);
		$dom=explode('/',$str);
		//$xstr='';
		//for($i=0;$i<count($dom)-1;$i++)if(empty($xstr)) $xstr=$dom[$i]; else $xstr.='/'.$dom[$i];
		//$str=$dom[count($dom)-1];
		$dom=$dom[0].'//'.$dom[2];
		$str=substr($str,strlen($dom));
		$xstr=$dom;
	}
	$str=str_replace('=','',base64_encode(code($str,$GLOBALS["e5uid"])));
	$str=str_replace('/','888C888',$str);
	$str=str_replace('+','999C999',$str);
	return $xstr.'/?AAC'.substr($str,0,strlen($str)).'CAA';
}

function deshurl($str,$set_get=1){
	if(strlen($str)>8){
		$xstr='';
		if(substr($str,0,7)=='http://' || substr($str,0,8)=='https'){
			$dom=explode('/',$str);
			$dom=$dom[0].'//'.$dom[2];
			$str=substr($str,strlen($dom));
			$xstr=$dom;
		}
		if(substr($str,0,5)=='/?AAC' && substr($str,strlen($str)-3,3)=='CAA'){
			$tmp=substr($str,5,strlen($str)-8);
			//$tmp=str_replace($tmp,'!','=');
			$tmp=str_replace('888C888','/',$tmp);
			$tmp=str_replace('999C999','+',$tmp);
			$res2=base64_decode($tmp);
			$res2=decode($res2,$GLOBALS["e5uid"]);
			//echo $res2;
			if(strstr($res2,'?') && $set_get){
				$res=explode('?',$res2);
				$res=$res[1];
				if(strstr($res,'&'))$res=explode('&',$res); else $res=Array($res);
				foreach($res AS $r){
					$r=explode('=',$r);
					//echo $r[0].' = '.$r[1].'<br>';						
					if(isset($r[0]) && isset($r[1])){
						$_GET[$r[0]]=$r[1];
					}
				}
			}
			return $xstr.$res2;
		} else return $xstr.$str;
	} else return $str;
}

function explode1($s1,$s2,$s3=''){
	if(empty($s3)) $in1=strpos5($s2,$s1);
	else $in1=strposA($s2,$s1,0,$s1,$s3);
	if($in1){
		$ar[0]=substr5($s2,0,$in1);
		//$ar[1]=substr5($s2,$in1+1,strlen5($s2)-$in1);
		$ar[1]=substr5($s2,$in1+strlen5($s1),strlen5($s2)-$in1-strlen5($s1));
		return $ar;
	} else return Array($s2);
}

function explodeD($sep,$str){	//SEP - массив
	$data=explode($sep[0],$str);
	for($i=1;$i<count($sep);$i++){
		$tmp=Array();
		foreach($data AS $d){
			$tmp=array_merge($tmp,explode($sep[$i],$d));
		}
		$data=$tmp;
	}
	return $data;
}

function get_all_tag($str,$t1,$t2){
	$pro_res2=Array();
	$u=0;$in=0;
	for($i=0;$i<strlen5($str);$i++) if (substr5($str,$i,strlen5($t1))==$t1){
		$pro_res='';		
		for($c=$i+strlen5($t1);$c<strlen5($str);$c++) if (substr5($str,$c,strlen5($t2))==$t2 && $in==0){
			$pro_res2[$u]=$pro_res;$u++;
			$i=$c+strlen5($t2)-1;
			break;
		} else {
			if (substr5($str,$c,strlen5($t1))==$t1) $in++;
			if (substr5($str,$c,strlen5($t2))==$t2) $in--;
			$pro_res=$pro_res.$str[$c];
		}
	}
	return $pro_res2;
}

function get_var($str){
	$arr=Array();
	/*$symb2='qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890_';
	$sybm=Array();for($i=0;$i<strlen5($symb2);$i++)$symb[$symb2[$i]]=1;*/
	global $var_symb;
	for($i=0;$i<strlen5($str);$i++){
		if($str[$i]=='$'){
			$vn='';
			for($i2=$i+1;$i2<strlen5($str);$i2++){
				if(!empty($var_symb[$str[$i2]])) $vn.=$str[$i2];
				else {$i=$i2; break;}
			}
			$arr[]=$vn;
		}
	}
	return $arr;
}

function get_global_str($arr){
	$glv='';
	$glv2=$arr;
	$glv3=Array();
	if(!empty($glv2) && count($glv2)!=0) for($i2=0;$i2<count($glv2);$i2++)if(!empty($glv2[$i2]) && empty($glv3[$glv2[$i2]])){
		if(empty($glv)) $glv='global $'.$glv2[$i2]; else $glv.=',$'.$glv2[$i2];
		$glv3[$glv2[$i2]]=1;
	}
	if(!empty($glv)) $glv.=';';
	return $glv;
}

function prepend_eval(&$str){
	$str=str_replace('\\','\\\\',$str);
	$str=str_replace('"','\"',$str);
}

function strpos_all($s1,$s2,$p=0){
	$res=Array();
	$x=strpos5($s1,$s2,$p);
	$y=($x===false);
	while(!$y){
		$res[]=$x;
		$x=strpos5($s1,$s2,$x+1);
		$y=($x===false);
	}
	return $res;
}

function strpos_all2($s1,$s2,$res=Array(),$p=0){
	if(empty($s1) || empty($s2)) return false;
	if($p!=0) $x=strpos5($s1,$s2,$p); else $x=strpos5($s1,$s2);
	$y=($x===false);
	while(!$y){
		$res[$x]=$s2;
		$x=strpos5($s1,$s2,$x+1);
		$y=($x===false);
	}
	return $res;
}

function strposA($s1,$s2,$p,$t1,$t2,$d1='',$d2='',$q=0){
	global $pspa;
	/*if(is_array($s1)){
		echo var_dump($s1);
		echo '<br>';
		debug_wayback(); 
		exit;
	}*/
	$l=$s1.'.'.$s2.'.'.$p.'.'.$t1.'.'.$t2.'.'.$d1.'.'.$d2.'.'.$q;
	if(isset($pspa[$l])){
		return $pspa[$l];
	}
	
	stf('strposA');
	$x=strpos($s1,$s2,$p);
	$y=($x===false);
	while(!$y){
		$d=substr5($s1,$p,$x-$p);
		if($d!=''){
			$r=strpos_all2($d,$t2,strpos_all2($d,$t1));			
			$a=0; if(!empty($r)) foreach($r AS $rr)if($rr==$t1) $a++; else $a--;
			if($a/*!=0*/>0){
			//if(!empty($r) && end($r)==$t1){
				$x=strpos($s1,$s2,$x+1);
				$y=($x===false);
				continue;
			}
			if(!empty($d1) && !empty($d2)){
				$r=strpos_all2($d,$d2,strpos_all2($d,$d1));
				$a=0; if(!empty($r)) foreach($r AS $rr)if($rr==$d1) $a++; else $a--;
				if($a/*!=0*/>0){
				//if(!empty($r) && end($r)==$d1){
					$x=strpos($s1,$s2,$x+1);
					$y=($x===false);
					continue;
				}		
			}
			if($q){
				$d=str_replace('\\\'','',$d);
				$d=str_replace('\\"','',$d);
				$r=strpos_all2($d,'"',strpos_all2($d,'\''));
				$q1=false;$q2=false;
				if(is_array($r)) ksort($r);
				if(!empty($r)) foreach($r AS $rr){
					if($rr=='\'' && !$q2) $q1=!$q1;
					if($rr=='"' && !$q1) $q2=!$q2;
				}
				if($q1 || $q2){
					$x=strpos($s1,$s2,$x+1);
					$y=($x===false);
					continue;
				}
				/*$r=substr_count($d,'\'');
				$r=$r-substr_count($d,'\\\'');
				$r2=substr_count($d,'"');
				$r2=$r2-substr_count($d,'\\"');
				if(($r % 2!=0 && $r!=0) || ($r2 % 2!=0 && $r2!=0)){
					$x=strpos($s1,$s2,$x+1);
					$y=($x===false);
					continue;
				}*/
			}
		}
		etf('strposA');
		global $no_cache;
		if(!$no_cache) $pspa[$l]=$x;
		return $x;
	}
	global $no_cache;
	if(!$no_cache) $pspa[$l]=false;
	etf('strposA');
	return false;
}

function expAenc($arr){
	$res='';
	foreach($arr AS $str){
		$res.=strlen5($str).'.'.$str;
	}
	return $res;
}

function expAdec($str){
	$sl=strlen5($str);
	$num='';
	$res=Array();
	$i2=0;
	for($i=0;$i<$sl;$i++){
		if($str[$i]!='.'){
			$num.=$str[$i];
		} else {
			$res[$i2]=substr5($str,$i+1,$num);
			$i=$i+$num;
			$num='';
			$i2++;
		}
	}
	return $res;
}

function do_explodeAcache(){//вызывать в конце работы скрипта
	global $pspa2,$cache_func2;
	if($cache_func2){
		check_dir(FTEMP.'explodeA',0);
		$f=fopen(FTEMP.'explodeA/all','w'); fwrite($f,serialize($pspa2)); fclose($f);
	}
}

function explodeA($s2,$s1,$t1,$t2,$firststop=0,$d1='',$d2='',$q=0,$start_at=0){//на стрпосах
	global $pspa2;
	if(empty($s1)) return Array();
	if(empty($s2)) return Array($s1);
	$l=$s1.'.'.$s2.'.'.$firststop.'.'.$t1.'.'.$t2.'.'.$d1.'.'.$d2.'.'.$q.'.'.$start_at;

	global $cache_func,$cache_func2;

	if(isset($pspa2[$l])){
		return $pspa2[$l];
	} else if($cache_func2){
		if(file_exists(FTEMP.'explodeA/all')) $pspa2=unserialize(file_get_contents(FTEMP.'explodeA/all'));
	}
	
	stf('explodeA');
	if($cache_func){
		$m5=md5($l);
		if(file_exists(FTEMP.'explodeA/'.$m5)){
			$pspa2[$l]=expAdec(file_get_contents(FTEMP.'explodeA/'.$m5));			
			etf('explodeA');
			return $pspa2[$l];
		}
	}
	$res=Array();
	$x=strpos5($s1,$s2,$start_at);
	$y=($x===false);
	$oldx=0;
	while(!$y){
		$d=substr5($s1,$oldx,$x-$oldx);
		if($d!=''){
			$r=strpos_all2($d,$t2,strpos_all2($d,$t1));			
			$a=0; if(!empty($r)) foreach($r AS $rr)if($rr==$t1) $a++; else $a--;
			if($a!=0){
				$x=strpos($s1,$s2,$x+1);
				$y=($x===false);
				continue;
			}
			if(!empty($d1) && !empty($d2)){
				$r=strpos_all2($d,$d2,strpos_all2($d,$d1));
				$a=0; if(!empty($r)) foreach($r AS $rr)if($rr==$d1) $a++; else $a--;
				if($a!=0){
					$x=strpos($s1,$s2,$x+1);
					$y=($x===false);
					continue;
				}		
			}
			if($q){
				$d=str_replace('\\\'','',$d);
				$d=str_replace('\\"','',$d);
				$r=strpos_all2($d,'"',strpos_all2($d,'\''));
				$q1=false;$q2=false;
				if(is_array($r)) ksort($r);
				if(!empty($r)) foreach($r AS $rr){
					if($rr=='\'' && !$q2) $q1=!$q1;
					if($rr=='"' && !$q1) $q2=!$q2;
				}
				if($q1 || $q2){
					$x=strpos($s1,$s2,$x+1);
					$y=($x===false);
					continue;
				}
			}
		}
		if($firststop){
			etf('explodeA');
			$res[0]=substr5($s1,0,$x);
			$res[1]=substr5($s1,$x+strlen5($s2));
			$pspa2[$l]=$res;
			if($cache_func){ check_dir(FTEMP.'explodeA',0); $f=fopen(FTEMP.'explodeA/'.$m5,'w'); fwrite($f,expAenc($res)); fclose($f);}
			return $res;
		}
		if(count($res)>0) $res[]=substr5($s1,$oldx,$x-$oldx);
		else $res[]=substr5($s1,0,$x);
		$oldx=$x+strlen5($s2);
		$x=strpos5($s1,$s2,$x+strlen5($s2));
		$y=($x===false);
	}
	if(count($res)==0){
		$res[]=$s1;
		$pspa2[$l]=$res;
		etf('explodeA');
		if($cache_func){ check_dir(FTEMP.'explodeA',0); $f=fopen(FTEMP.'explodeA/'.$m5,'w'); fwrite($f,expAenc($res)); fclose($f);}
		return $res;
	}
	if($oldx<=strlen5($s1)){
		$res[]=substr5($s1,$oldx);
	}
	$pspa2[$l]=$res;
	etf('explodeA');

	//echo '<div><b>'.$s1.' - Exp: '.$s2.' - FST: '.$firststop.' - q: '.$q.' - t1: '.$t1.' - t2: '.$t2.' - d1: '.$d1.' - d2: '.$d2.'</b></div><ul>';
	//foreach($res AS $r) echo '<li>&nbsp;'.$r.'</li>';
	//echo '</ul><br>';

	if($cache_func){ check_dir(FTEMP.'explodeA',0); $f=fopen(FTEMP.'explodeA/'.$m5,'w'); fwrite($f,expAenc($res)); fclose($f);}
	return $res;
}

function explodeB($sep,$str,$tag1='',$tag2='',$firststop=0,$tag1b='',$tag2b=''){
	global $pexp2;
	$l=implode('|',$sep).'.'.$str.'.'.$tag1.'.'.$tag2.'.'.$firststop.$tag1b.$tag2b;
	if(!isset($pexp2[$l])){
		stf('explodeB');
		$mi=0;
		$mo=0;
		$res=Array();$ci=0;
		for($i=0;$i<strlen5($str);$i++){
			if($mi==0) for($i2=0;$i2<count($sep);$i2++){
				$r=false;
				if(substr5($str,$i,strlen5($sep[$i2]))==$sep[$i2] && $mi==0 && $mo==0){
					$i=$i+strlen5($sep[$i2])-1+1;
					if($firststop){
						$res[1]->str=substr5($str,$i+1,strlen5($str)-$i);
						$res[0]->sep=$sep[$i2];
						etf('explodeA');
						$pexp2[$l]=$res;
						return $res;
					}
					$res[$ci]->sep=$sep[$i2];
					$ci++;
					$r=true;
					break;
				}
				if($r) continue;
			}
			if(!empty($tag1) && substr5($str,$i,strlen5($tag1))==$tag1) $mi++;
			if(!empty($tag2) && substr5($str,$i,strlen5($tag2))==$tag2) $mi--;
			if(!empty($tag1b) && substr5($str,$i,strlen5($tag1b))==$tag1b) $mo++;
			if(!empty($tag2b) && substr5($str,$i,strlen5($tag2b))==$tag2b) $mo--;
			if(empty($res[$ci])) $res[$ci]->str=$str[$i]; else $res[$ci]->str.=$str[$i];
		}
	} else {
		return $pexp2[$l];
	}
	$pexp2[$l]=$res;
	etf('explodeB');
	if($ci==0) return false;
	return $res;	
}

function as_html($string){
	return nl2br(htmlspecialchars(trim(stripslashes($string))));
}

function as_text($string){
	return trim(stripslashes($string));
}

function dblchar($str,$num){
	$res='';
	for($i=0;$i<$num;$i++)$res.=$str;
	return $res;
}

function array_to_string($arr){
	if(is_object($arr) && isset($arr->id)) $arr=$arr->id;
	if(is_object($arr) && isset($arr->rid)) $arr=$arr->rid;
	return serialize($arr);
	/*$r='';
	if(is_array($arr)){ $f='^!'; $f2='^.';}
	else if(is_object($arr)){ $f='^*'; $f2='^,';}
	else return $arr;
	//if(empty($arr)) return '';
	foreach($arr AS $var=>$value){
		if(!empty($r)) $r.='^;';
		$r.=$var.'^=';
		if(is_array($value)){
			$r.=array_to_string($value);
		} else if(is_object($value)) {
			$r.=array_to_string($value);
		} else $r.=$value;
	}
	return $f.$r.$f2;*/
}

function string_to_array($str){
	return unserialize($str);
	/*if(substr($str,0,2)=='^!'){
		$type=0;
		$r=Array();
	}	
	if(substr($str,0,2)=='^*'){
		$type=1;
		$r->tmpvar=1;
		unset($r->tmpvar);
	}
	if(!isset($type)) return $str;
	$str=substr($str,2,strlen($str)-2);
	if($str=='') return $r;
	while(substr($str,strlen($str)-2,2)=='^.') $str=substr($str,0,strlen($str)-2);
	while(substr($str,strlen($str)-2,2)=='^,') $str=substr($str,0,strlen($str)-2);	
	$str=explodeA('^;',$str,'^!','^.',0,'^*','^,',0);
	foreach($str AS $st){
		$st2=explodeA('^=',$st,'^*','^,',1,'^!','^.',0);
		if(empty($st2[1])) $st2[1]='';
		if($type==0) $r[$st2[0]]=string_to_array($st2[1]);
		if($type==1){
			global $tmp,$tmp2;
			$tmp=$r;
			$tmp2=string_to_array($st2[1]);
			eval('global $tmp,$tmp2; $tmp->'.$st2[0].'=$tmp2;');
			$r=$tmp;
		}
	}
	if(!isset($r)) return;
	return $r;*/
}

function Utf8ToWin($fcontents) {
    $out = $c1 = '';
    $byte2 = false;
    for ($c = 0;$c < strlen5($fcontents);$c++) {
        $i = ord($fcontents[$c]);
        if ($i <= 127) {
            $out .= $fcontents[$c];
        }
        if ($byte2) {
            $new_c2 = ($c1 & 3) * 64 + ($i & 63);
            $new_c1 = ($c1 >> 2) & 5;
            $new_i = $new_c1 * 256 + $new_c2;
            if ($new_i == 1025) {
                $out_i = 168;
            } else {
                if ($new_i == 1105) {
                    $out_i = 184;
                } else {
                    $out_i = $new_i - 848;
                }
            }
            // UKRAINIAN fix
            switch ($out_i){
                case 262: $out_i=179;break;// і
                case 182: $out_i=178;break;// І 
                case 260: $out_i=186;break;// є
                case 180: $out_i=170;break;// Є
                case 263: $out_i=191;break;// ї
                case 183: $out_i=175;break;// Ї
                case 321: $out_i=180;break;// ґ
                case 320: $out_i=165;break;// Ґ
            }
            $out .= chr($out_i);
            
            $byte2 = false;
        }
        if ( ( $i >> 5) == 6) {
            $c1 = $i;
            $byte2 = true;
        }
    }
    return $out;
}

function prepstr($str){
	$GLOBALS["str"]='';
	eval('$GLOBALS["str"]="'.$str.'";');
	return $GLOBALS["str"];
}

function str2num($str){
	$res=0;
	for($i=0;$i<strlen5($str);$i++)$res+=ord($str[$i])*$i;
	return $res;
}

function del1($arr){
	array_shift($arr);
	return $arr;
}

function del_tag($str,$t1,$t2){
	$t=get_tag($str,$t1,$t2);
	while($t!=''){
		$str=str_replace($t1.$t.$t2,'',$str);
		$t=get_tag($str,$t1,$t2);
	}
	return $str;
}

function get_tag($str,$t1,$t2,$continue=false,$start=0){
	global $tag_cache;
	if($continue && !empty($tag_cache[$str])) $start=$tag_cache[$str];
	//здесь нужно сделать более быстрый вариант get_tag на strpos-ах
	stf('get_tag');
	$x=strpos($str,$t1,$start);
	if($x===false){etf('get_tag'); return '';}
	$l1=strlen($t1);
	$l2=strlen($t2);
	$y=strpos($str,$t2,$x+$l1/*-1*/);
	if($y===false){etf('get_tag'); return '';}
	$z=strpos($str,$t1,$x+$l1/*-1*/);
	while($z!==false && $z<$y){
		$y=strpos($str,$t2,$y+$l2/*-1*/);
		if($y===false){etf('get_tag'); return get_tagB($str,$t1,$t2,$start);}
		$z=strpos($str,$t1,$z+$l1/*-1*/);
	}
	$tag_cache[$str]=$y+strlen($t2);
	etf('get_tag');
	return substr5($str,$x+$l1,$y-$x-$l1);
}

function get_tagO($str,$t1,$t2,$continue=false){
	global $tag_cache;
	$uu=0;
	$start=0;
	if($continue && !empty($tag_cache[$str])) $start=$tag_cache[$str];
	for($i=$start;$i<strlen5($str);$i++) if (substr5($str,$i,strlen5($t1))==$t1){
		$pro_res='';		
		for($c=$i+strlen5($t1);$c<strlen5($str);$c++) if (substr5($str,$c,strlen5($t2))==$t2 && $uu==0){
			$tag_cache[$str]=$c+strlen5($t2)+1;//+1 под сомненьем
			return $pro_res;
		} else {
			if(substr5($str,$c,strlen5($t1))==$t1) $uu++;
			if(substr5($str,$c,strlen5($t2))==$t2) $uu--;
			$pro_res=$pro_res.$str[$c];
		}
	}
	return '';
}

function get_tagA($str,$t1,$t2,$e1,$e2){
	$uu=0;$uu2=0;
	for($i=0;$i<strlen5($str);$i++) if (substr5($str,$i,strlen5($t1))==$t1){
		$pro_res='';		
		for($c=$i+strlen5($t1);$c<strlen5($str);$c++) if (substr5($str,$c,strlen5($t2))==$t2 && $uu==0 && $uu2==0)
			return $pro_res;
		else {
			if(substr5($str,$c,strlen5($t1))==$t1) $uu++;
			if(substr5($str,$c,strlen5($t2))==$t2) $uu--;
			if(substr5($str,$c,strlen5($e1))==$e1) $uu2++;
			if(substr5($str,$c,strlen5($e2))==$e2) $uu2--;
			$pro_res=$pro_res.$str[$c];
		}
	}
	return '';
}

function rstrstr($haystack,$needle, $start=0){
        return substr($haystack, $start,strpos($haystack, $needle));
}

function get_tagB($str,$t1,$t2,$start=0){//ищет вначале t2, потом ищет t1
	$right=rstrstr($str,$t2);
	if(empty($right)) return '';
	if($right<$start) return '';
	$left=strrpos($right,$t1);
	if($left===false) return '';
	if($left<$start) return '';
	return substr($right,$left+strlen($t1));
}

function page_uin($url=''){
	if(empty($url)) $url=$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"];
	$i=52000-ABS(str2num($url) % 52000);
	return $i;
}

function xml_row_parse($data){
	$sld=strlen($data);
	for($i=0;$i<$sld;$i++)if($data[$i]=='<'){	
		$x=strpos($data,'>',$i+1);
		if($x!==false){
			$y=strpos($data,'</',$x+1);
			if($y!==false){
				$z=strpos($data,'>',$y+1);
				if($z!==false){
					$name=substr($data,$i+1,$x-$i-1);
					$text=substr($data,$x+1,$y-$x-1);
					$result->{$name}=$text;
					$i=$z+1;
				}
			}
		}
	}
	if(isset($result)) return $result; else return;
}

function stf($fname){
	global $debug,$debug2;
	if(!$debug || !$debug2) return;
	global $ftime,$ftime2,$fproc,$fspace,$fcount,$ftime4,$ftime5,$ftime6,$curf;
	$curf=$fname;
	if(!isset($ftime4[$fname]) || $ftime4[$fname]=='ok') $ftime4[$fname]=1; else $ftime4[$fname]=$ftime4[$fname]+1;
	$mtime = microtime(1); //$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];
	$fproc.=$fspace.$fname.'<br>';
	$fspace.='-';
	if(empty($GLOBALS["tqrow"])) $GLOBALS["tqrow"]=0;
	if(empty($GLOBALS["aquery"])) $GLOBALS["aquery"]=0;
	$ftime2[$fspace]->start3=$GLOBALS["tqrow"];
	$ftime2[$fspace]->start2=count($GLOBALS["aquery"]);
	$ftime2[$fspace]->start=$mtime;
	$ftime2[$fspace]->func=$fname;
	if(empty($fcount[$fname])) $fcount[$fname]=1; else $fcount[$fname]++;
	if(empty($ftime[$fname])){ $ftime[$fname]->time=0.001; $ftime[$fname]->query=0; $ftime[$fname]->row=0;}
}

function etf($fname){
	global $debug,$debug2;
	if(!$debug || !$debug2) return;
	global $ftime,$ftime2,$fproc,$fspace,$ftime4,$ftime5,$ftime6;
	if(!isset($ftime4[$fname]) || $ftime4[$fname]=='ok') $ftime4[$fname]=-1; else $ftime4[$fname]=$ftime4[$fname]-1;
	if($ftime4[$fname]==0) $ftime4[$fname]='ok';
	$mtime = microtime(1); //$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];
	if(empty($ftime2[$fspace])) return '';
	$time=$mtime - $ftime2[$fspace]->start;
	$query=count($GLOBALS["aquery"]) - $ftime2[$fspace]->start2;
	$row=$GLOBALS["tqrow"] - $ftime2[$fspace]->start3;
	if(isset($ftime[$fname])){
		$ftime[$fname]->time=$ftime[$fname]->time+$time;
		$ftime[$fname]->query=$ftime[$fname]->query+$query;
		$ftime[$fname]->row=$ftime[$fname]->row+$row;
	}
	if($fspace!='') $fspace=substr5($fspace,0,strlen5($fspace)-1);
	if($fspace!=''){
		$ftime[$ftime2[$fspace]->func]->time=$ftime[$ftime2[$fspace]->func]->time-$time;
		$ftime[$ftime2[$fspace]->func]->query=$ftime[$ftime2[$fspace]->func]->query-$query;
		$ftime[$ftime2[$fspace]->func]->row=$ftime[$ftime2[$fspace]->func]->row-$row;
	}
}

function spf($pname){
	global $debug,$debug2;
	if(!$debug || !$debug2) return;

	global $ptime2, $pspace, $pcount, $ptime;
	$mtime = microtime(1);//$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];
	if(empty($pspace)) $pspace='';
	$pspace.='-';
	$ptime2[$pspace]->start=$mtime;
	$ptime2[$pspace]->func=$pname;
	if(empty($pcount[$pname])) $pcount[$pname]=1; else $pcount[$pname]++;
	if(empty($ptime[$pname])) $ptime[$pname]->time=0.001;
}

function epf($pname){
	global $debug,$debug2;
	if(!$debug || !$debug2) return;
	
	global $ptime2, $pspace, $ptime;
	$mtime = microtime(1);//$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];
	if(empty($ptime2[$pspace])) return '';
	$time=$mtime - $ptime2[$pspace]->start;
	if(isset($ptime[$pname])){
		$ptime[$pname]->time=$ptime[$pname]->time+$time;
	}
	if($pspace!='') $pspace=substr5($pspace,0,strlen5($pspace)-1);
	if($pspace!=''){
		$ptime[$ptime2[$pspace]->func]->time=$ptime[$ptime2[$pspace]->func]->time-$time;
	}
}

function clean_html($content){
	return strip_tags($content);
}

function parseInt($str){
	$result='';
	$point=false;
	for($i=0;$i<strlen($str);$i++){
		if(ctype_digit($str[$i])) $result.=$str[$i];
		else if($str[$i]=='.' && !$point){
			$point=true;
			$result.='.';
		}
	}
	if(empty($result)) $result='0';
	return $result;
}

function get_code4($length){
	$result='';
	for($i=0;$i<$length;$i++) $result.=chr(rand(0,255));
	return $result;
}

function get_code3($length){
	$result='';
	$a='qwrtpsdfghjklzxcvbnm';
	$b='eyuioa';
	$c='0123456789';
	for($i=0;$i<$length;$i++)if($i<$length/4*3){
		if($i%2==0) $result.=$a[rand(0,strlen5($a)-1)]; else $result.=$b[rand(0,strlen5($b)-1)];
	} else $result.=$c[rand(0,strlen5($c)-1)];
	return $result;
}

function get_code2($length){
	$result='';
	$a='qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890';
	for($i=0;$i<$length;$i++) $result.=$a[rand(0,strlen5($a)-1)];
	return $result;
}

function get_code($length){
	$result='';
	for($i=0;$i<$length;$i++) $result.=rand(0,9);
	return $result;
}

function get_str_num($price,$mon1,$mon2,$mon3){
	$price="$price";
	$mon[0]=$mon1;//один. пример: рубль
	$mon[1]=$mon2;//два-четыре. пример: рубля
	$mon[2]=$mon3;//пять-десять. пример: рублей
	$result=$mon[0];
	if(strlen5($price)>1 && $price[strlen5($price)-2]==1 && $price[strlen5($price)-1]<5){
		$result=$mon[2];
	} else if (strlen5($price)>0 && $price[strlen5($price)-1]==1) $result=$mon[0];
	else if (strlen5($price)>0 && $price[strlen5($price)-1]>1 && $price[strlen5($price)-1]<5) $result=$mon[1];
	else $result=$mon[2];
	if($price==0) $result=$mon[2];
	return $result;
}

function get_special_chars($res){
	$res=str_replace('&amp;','&',$res);
	$res=str_replace('&quot;','"',$res);
	$res=str_replace('&lt;','<',$res);
	$res=str_replace('&gt;','>',$res);
	$res=str_replace('&nbsp;',' ',$res);
	return $res;
}

function chop_all($res){
	while(strpos5($res,'  ',0)!=0) $res=str_replace('  ',' ',$res);
	return $res;
}

function decode($str,$pwl=''){
	$x=strlen5($str);
	for($i=strlen5($str)-1;$i>=0;$i--){
		if($pwl!=''){
			$z=ord($pwl[strlen5($pwl) - $i % strlen5($pwl) -1]);
			$int=$i+$z;
			if($int>=$x-1){
				$z=$int-($x-$i-1);
				if($z>$x-1)$int=$z % $x;
				else $int=$z-1;
			}
			$z=$str[$i];
			@$str[$i]=$str[$int];		//тут почему-то может возникнуть ошибка Notice: Uninitialized string offset: -1 
			@$str[$int]=$z;
		}
		if($pwl=='') $int=ord($str[$i])-25;
		else $int=ord($str[$i])-ord($pwl[$i % strlen5($pwl)]);
		if ($int</*255*/0) $int=$int+255+1;
		$str[$i]=chr($int);
	}
	return $str;
}

function code($str,$pwl=''){
	$x=strlen5($str);
	for($i=0;$i<strlen5($str);$i++){
		if($pwl=='') $int=ord($str[$i])+25;
		else $int=ord($str[$i])+ord($pwl[$i % strlen5($pwl)]);
		if ($int>255) $int=$int-255-1;
		$str[$i]=chr($int);
		if($pwl!=''){
			$z=ord($pwl[strlen5($pwl) - $i % strlen5($pwl) -1]);
			$int=$i+$z;
			if($int>=$x-1){
				$z=$int-($x-$i-1);
				if($z>$x-1)$int=$z % $x;
				else $int=$z-1;
			}
			//убрал -1 и ерроры ушли...
			$z=$str[$i];
			$str[$i]=$str[$int];
			$str[$int]=$z;
		}
	}
	return $str;
}

function soft_code($str,$pwl){
	$x=strlen($str);
	$y=strlen($pwl);
	for($i=0;$i<$x;$i++){
		$z=ord($pwl[$y - $i % $y-1]);
		$int=$i+$z;
		if($int>=$x-1){
			$z=$int-($x-$i-1);
			if($z>$x-1)$int=$z % $x;
			else $int=$z-1;
		}
		$z=$str[$i];
		$str[$i]=$str[$int];
		$str[$int]=$z;
	}
	return $str;
}

function soft_code2($str,$pwl,$deep_coding=true){
	$x=strlen($str);
	$y=strlen($pwl);
	$az=1;
	$azs='';
	for($i=0;$i<strlen($str);$i++){
		if($deep_coding) $azs=$str[$i].$azs;
		$int=ord($str[$i])+ord($pwl[$i % strlen5($pwl)]);
		if ($int>255) $int=$int-255-1;
		$str[$i]=chr($int);
		$az++;
		if($az==$y+1 && $deep_coding){
			$pwl=soft_code2($pwl,$azs,false);
			$az=1;
			$azs='';
		}
	}
	return $str;
}

function e_code_diff($a,$b,$c,$n){
	// Получаем значение ASCII символа
	$x=ord($a);
	$y=ord($b);
	// Применяем чётность
	if(!$c) $y=-$y;
	// Складываем значения
	$x+=$y;
	// Проверяем лимиты
	if($x>=$n) $x=($x % $n);
	if($x<=-$n) $x=($x % $n);
	// Сдвиг справа
	if($x<0) $x+=$n;
	return $x;	
}

function e_code($str,$pwl){
	$x=strlen($str);
	$y=strlen($pwl);
	$pwl2=$pwl;
	if($y==0) return $str;
	// Перестановка символов
	for($i=0;$i<$x;$i++){
		$a=$pwl2[$i % $y];	// Берём нужный символ из хеш-пароля
		$c=($i % 2==0); // Определяем чётность индекса цикла
		// Меняем символы местами
		$d=e_code_diff($i,$a,$c,$x); // Получаем сдвиг на основе символа хеш-пароля
		$t=$str[$d]; // Меняем местами исходный символ с символом сдвига
		$str[$d]=$str[$i];
		$str[$i]=$t;
		// Генерируем следующее значение символа хеш-пароля с помощью текущего значения, индекса цикла и его чётности
		$a=e_code_diff($a,$i,$c,256);
		$a=chr($a);
		$pwl2[$i % $y]=$a;
	}
	// Сдвиг по ASCII
	for($i=0;$i<$x;$i++){
		$a=$pwl[$i % $y]; // Берём нужный символ из хеш-пароля
		$b=$str[$i];
		$z=$b;
		$c=($i % 2==0); // Определяем чётность индекса цикла
		// Делаем сдвиг
		$b=e_code_diff($b,$a,$c,256); // Определяем позицию сдвига от 0 до 256 с помощью текущего символа хеш-пароля и чётности индекса цикла
		$b=chr($b);
		$a=e_code_diff($a,/*$b*/$z,$c,256); // Определям следующее значение символа хеш-пароля с помощью текущего шифрованного символа и чётности цикла
		$a=chr($a);
		// Запись значений
		$str[$i]=$b;
		$pwl[$i % $y]=$a;
	}
	return $str;
}

function e_decode($str,$pwl){
	$x=strlen($str);
	$y=strlen($pwl);
	$pwl2=$pwl;
	if($y==0) return $str;
	// Сдвиг по ASCII и генерация необходимого для обратного перемещения символов хеш-пароля
	for($i=0;$i<$x;$i++){
		$a=$pwl[$i % $y]; // Берём нужный символ из хеш-пароля
		$b=$str[$i];
		$z=$b;
		$c=($i % 2==0); // Определяем чётность цикла
		
		$b=e_code_diff($b,$a,!$c,256); // Осуществляем обратный сдвиг по ASCII (используя обратную чётность индекса цикла и текущий символ из хеш-пароля)
		$b=chr($b);
		
		$a=e_code_diff($a,/*$z*/$b,$c,256); // Определяем следующее значение символа хеш-пароля с помощью шифрованного символа и чётности цикла
		$a=chr($a);
		
		// Запись значения
		$str[$i]=$b;
		$pwl[$i % $y]=$a;
		
		// Подготовка хеш-пароля для обратной перестановки
		$a=$pwl2[$i % $y];
		$a=e_code_diff($a,$i,$c,256);
		$a=chr($a);
		$pwl2[$i % $y]=$a;
	}
	// Обратная перестановка символов
	for($i=$x-1;$i>=0;$i--){
		$a=$pwl2[$i % $y]; // Берём нужный символ из хеш-пароля
		$c=($i % 2==0); // Определяем чётность индекса цикла
		
		$a=e_code_diff($a,$i,!$c,256); // Определяем новый символ для хеш-пароля
		$a=chr($a);
		$pwl2[$i % $y]=$a;	
		
		$d=e_code_diff($i,$a,$c,$x); // Определяем сдвиг для символа
		// Меняем символы местами
		$t=$str[$d];
		$str[$d]=$str[$i];
		$str[$i]=$t;
	}
	return $str;
}

function lzw_compress($string) {
	// compression
	$dictionary = array_flip(range("\0", "\xFF"));
	$word = "";
	$codes = array();
	for ($i=0; $i <= strlen($string); $i++) {
		$x = $string[$i];
		if (strlen($x) && isset($dictionary[$word . $x])) {
			$word .= $x;
		} elseif ($i) {
			$codes[] = $dictionary[$word];
			$dictionary[$word . $x] = count($dictionary);
			$word = $x;
		}
	}
	
	// convert codes to binary string
	$dictionary_count = 256;
	$bits = 8; // ceil(log($dictionary_count, 2))
	$return = "";
	$rest = 0;
	$rest_length = 0;
	foreach ($codes as $code) {
		$rest = ($rest << $bits) + $code;
		$rest_length += $bits;
		$dictionary_count++;
		if ($dictionary_count > (1 << $bits)) {
			$bits++;
		}
		while ($rest_length > 7) {
			$rest_length -= 8;
			$return .= chr($rest >> $rest_length);
			$rest &= (1 << $rest_length) - 1;
		}
	}
	return $return . ($rest_length ? chr($rest << (8 - $rest_length)) : "");
}

function simple_string($src){
	$res='';
	$c='qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM123456789';
	$src=(string)$src;
	for($i=0;$i<strlen($src);$i++){
		$o=ord($src[$i]);
		$res.=$c[(integer)floor($o/16)].$c[$o%16];
	}
	return $res;
}

function desimple_string($src){
	$res='';
	$c='qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM123456789';
	for($i=0;$i<strlen($c);$i++) $c2[$c[$i]]=$i;
	for($i=0;$i<strlen($src);$i=$i+2){
		$a=$c2[$src[$i]];
		$b=$c2[$src[$i+1]];
		$res.=chr($a*16+$b);
	}
	return $res;
}

function win_utf8($str) { 
	if (function_exists('mb_convert_encoding')) return mb_convert_encoding($str, 'utf-8', 'windows-1251');
	if (function_exists('iconv')) return iconv('windows-1251', 'utf-8', $str);
	$win1251utf8 = array( 
	"\xC0"=>"А","\xC1"=>"Б","\xC2"=>"В","\xC3"=>"Г","\xC4"=>"Д","\xC5"=>"Е","\xA8"=>"Ё","\xC6"=>"Ж","\xC7"=>"З","\xC8"=>"И","\xC9"=>"Й","\xCA"=>"К","\xCB"=>"Л","\xCC"=>"М",
	"\xCD"=>"Н","\xCE"=>"О","\xCF"=>"П","\xD0"=>"Р","\xD1"=>"С","\xD2"=>"Т","\xD3"=>"У","\xD4"=>"Ф","\xD5"=>"Х","\xD6"=>"Ц","\xD7"=>"Ч","\xD8"=>"Ш","\xD9"=>"Щ","\xDA"=>"Ъ",
	"\xDB"=>"Ы","\xDC"=>"Ь","\xDD"=>"Э","\xDE"=>"Ю","\xDF"=>"Я","\xE0"=>"а","\xE1"=>"б","\xE2"=>"в","\xE3"=>"г","\xE4"=>"д","\xE5"=>"е","\xB8"=>"ё","\xE6"=>"ж","\xE7"=>"з",
	"\xE8"=>"и","\xE9"=>"й","\xEA"=>"к","\xEB"=>"л","\xEC"=>"м","\xED"=>"н","\xEE"=>"о","\xEF"=>"п","\xF0"=>"р","\xF1"=>"с","\xF2"=>"т","\xF3"=>"у","\xF4"=>"ф","\xF5"=>"х",
	"\xF6"=>"ц","\xF7"=>"ч","\xF8"=>"ш","\xF9"=>"щ","\xFA"=>"ъ","\xFB"=>"ы","\xFC"=>"ь","\xFD"=>"э","\xFE"=>"ю","\xFF"=>"я"); 
	return strtr($str, $win1251utf8); 
}

function utf8_win($s)
{
	if (function_exists('mb_convert_encoding')) return mb_convert_encoding($s, 'windows-1251', 'utf-8');
	if (function_exists('iconv')) return iconv('utf-8', 'windows-1251', $s);
	$s=strtr($s,array(
	"\xD0\xB0"=>"а", "\xD0\x90"=>"А",	"\xD0\xB1"=>"б", "\xD0\x91"=>"Б",	"\xD0\xB2"=>"в", "\xD0\x92"=>"В",	"\xD0\xB3"=>"г", "\xD0\x93"=>"Г",	"\xD0\xB4"=>"д", "\xD0\x94"=>"Д",	"\xD0\xB5"=>"е", "\xD0\x95"=>"Е",	"\xD1\x91"=>"ё", "\xD0\x81"=>"Ё",	"\xD0\xB6"=>"ж", "\xD0\x96"=>"Ж",	"\xD0\xB7"=>"з", "\xD0\x97"=>"З",	"\xD0\xB8"=>"и", "\xD0\x98"=>"И",	"\xD0\xB9"=>"й", "\xD0\x99"=>"Й",	"\xD0\xBA"=>"к", "\xD0\x9A"=>"К",	"\xD0\xBB"=>"л", "\xD0\x9B"=>"Л",	"\xD0\xBC"=>"м", "\xD0\x9C"=>"М",	"\xD0\xBD"=>"н", "\xD0\x9D"=>"Н",	"\xD0\xBE"=>"о", "\xD0\x9E"=>"О",	"\xD0\xBF"=>"п", "\xD0\x9F"=>"П",	"\xD1\x80"=>"р", "\xD0\xA0"=>"Р",	"\xD1\x81"=>"с", "\xD0\xA1"=>"С",	"\xD1\x82"=>"т", "\xD0\xA2"=>"Т",	"\xD1\x83"=>"у", "\xD0\xA3"=>"У",	"\xD1\x84"=>"ф", "\xD0\xA4"=>"Ф",	"\xD1\x85"=>"х", "\xD0\xA5"=>"Х",	"\xD1\x86"=>"ц", "\xD0\xA6"=>"Ц",	"\xD1\x87"=>"ч", "\xD0\xA7"=>"Ч",	"\xD1\x88"=>"ш", "\xD0\xA8"=>"Ш",	"\xD1\x89"=>"щ", "\xD0\xA9"=>"Щ",	"\xD1\x8A"=>"ъ", "\xD0\xAA"=>"Ъ",	"\xD1\x8B"=>"ы", "\xD0\xAB"=>"Ы",	"\xD1\x8C"=>"ь", "\xD0\xAC"=>"Ь",
	"\xD1\x8D"=>"э", "\xD0\xAD"=>"Э",	"\xD1\x8E"=>"ю", "\xD0\xAE"=>"Ю",	"\xD1\x8F"=>"я", "\xD0\xAF"=>"Я",
	));
	return $s;
}

function is_num($str){
	$num='0123456789';
	for($i=0;$i<strlen5($str);$i++) if(!strpos5($num,$str[$i])) return false;
	return true;
}

function procent($current,$max){
	if($max/100<>0) return floor($current/($max/100));
	else return 0;
}

function get_max_str($str1,$str2){
	if (strlen5($str1)>strlen5($str2)) return $str1; else return $str2;
}

function plagiat($str,$str2){
	similar_text($str,$str2,$percent);
	return round($percent);
}

function search($t1,$t2){
	if(strlen5($t1)>strlen5($t2)){
		$tmp=$t1;
		$t1=$t2;
		$t2=$tmp;
	}
	return substr_count($t1,$t2);
}

function search2($content, $stand){
	$content=strtolower5($content);
	$stand=strtolower5($stand);
	if(strlen5($stand)>strlen5($content)){
		$tmp=$stand;
		$stand=$content;
		$content=$tmp;
	}
	return substr_count($stand,$content);
}

function ss21($content,$start,$end,$r1,$r2){
	if(strpos5($content,$end)!=0 && strpos5($content,$end)<strpos5($content,$start)) $content=$r1.$content;
	$start=search2($content,$start);
	$end=search2($content,$end);
	if($end<$start) for($i=0;$i<$start-$end;$i++) $content=$content.$r1;
	if($end>$start) for($i=0;$i<$end-$start;$i++) $content=$r2.$content;
	return $content;
}

function safe_content($content){
	$in1=0;$bool=false;
	$tmp=$content;
	for($i=strlen5($content)-1;$i>0;$i--){
		if($in1==0 && $content[$i]!='>') break; else {
			if($content[$i]=='>') $in1++;
			if($content[$i]=='<'){
				$in1--;
				if($in1==0 && !$bool) $tmp=substr5($content,0,$i);
				$bool=false;
			}
			if($content[$i]=='/' && $in1!=0) $bool=true;
		}
	}
	$content=$tmp;
	
	$content=ss21($content,'<b','</b','</b>','<b>');
	$content=ss21($content,'<strong','</strong','</strong>','<strong>');
	$content=ss21($content,'<span','</span','</span>','<span>');
	$content=ss21($content,'<div','</div','</div>','<div>');
	$content=ss21($content,'<p','</p','</p>','<p>');
	$content=ss21($content,'<h1','</h1','</h1>','<h1>');
	$content=ss21($content,'<h2','</h2','</h2>','<h2>');
	$content=ss21($content,'<h3','</h3','</h3>','<h3>');
	$content=ss21($content,'<h4','</h4','</h4>','<h4>');
	$content=ss21($content,'<h5','</h5','</h5>','<h5>');
	$content=ss21($content,'<h6','</h6','</h6>','<h6>');
	$content=ss21($content,'<a ','</a','</a>','<a>');
	$content=ss21($content,'<font','</font','</font>','<font>');
	$content=ss21($content,'<li','</li','</li>','<li>');
	$content=ss21($content,'<ul','</ul','</ul>','<ul>');
	$content=ss21($content,'<ol','</ol','</ol>','<ol>');
	$content=ss21($content,'<td','</td','</td>','<td>');
	$content=ss21($content,'<tr','</tr','</tr>','<tr>');
	$content=ss21($content,'<table','</table','</table>','<table>');
	if(  ( search2($content,'<tr') || search2($content,'</tr') || search2($content,'<td') || search2($content,'</td') )  &&  !search2($content,'<table')) $content='<table>'.$content;
	if(  ( search2($content,'<tr') || search2($content,'</tr') || search2($content,'<td') || search2($content,'</td') )  &&  !search2($content,'</table')) $content=$content.'</table>';
	return $content;
}

function crop_longtext($text,$length,$separator='...'){
	if(strlen5($text)>$length) return mb_substr($text,0,$length,'UTF-8').$separator; else return $text;
}

function longfix($str,$max=45){
	$mws=0;
	$res='';
	$str=utf8_win($str);
	for($i=0;$i<strlen5($str);$i++){
		$mws++;
		if($str[$i]==' ') $mws=0;
		$res.=$str[$i];
		if($mws>$max){
			$res.=' ';
			$mws=0;
		}
	}
	$res=win_utf8($res);	
	return $res;
}

function is_bukva($buk){
	$buk=strtolower5($buk);
	switch ($buk){
		case 'й':return true;case 'ц':return true;case 'у':return true;case 'к':return true;case 'е':return true;case 'н':return true;case 'г':return true;case 'ш':return true;case 'щ':return true;case 'з':return true;case 'х':return true;case 'ъ':return true;case 'ф':return true;case 'ы':return true;case 'в':return true;case 'а':return true;case 'п':return true;case 'р':return true;case 'о':return true;case 'л':return true;case 'д':return true;case 'ж':return true;case 'э':return true;case 'я':return true;case 'ч':return true;case 'с':return true;case 'м':return true;case 'и':return true;case 'т':return true;case 'ь':return true;case 'б':return true;case 'ю':return true;case 'ё':return true;
		case 'q':return true;case 'w':return true;case 'e':return true;case 'r':return true;case 't':return true;case 'y':return true;case 'u':return true;case 'i':return true;case 'o':return true;case 'p':return true;case 'a':return true;case 's':return true;case 'd':return true;case 'f':return true;case 'g':return true;case 'h':return true;case 'j':return true;case 'k':return true;case 'l':return true;case 'z':return true;case 'x':return true;case 'c':return true;case 'v':return true;case 'b':return true;case 'n':return true;case 'm':return true;
		case '-':return true;case '!':return true;case '.':return true;case '«':return true;case '»':return true;
	}
	return false;
}

function comillas($str){
	$is_html=0;
	$str=utf8_win($str);
	$l=utf8_win('«');
	$r=utf8_win('»');	
	for($i=0;$i<strlen5($str);$i++){
		if($str[$i]=='<')$is_html=1;
		if($str[$i]=='>')$is_html=0;
		if($is_html==0){
			if($i==0 && $str[$i]=='"') $str[$i]=$l;
			else if($i==(strlen5($str)-1) && $str[$i]=='"') $str[$i]=$r;
			else if($i>0 && ($i<strlen5($str)-1) && $str[$i]=='"' && is_bukva(win_utf8($str[$i+1])) && !is_bukva(win_utf8($str[$i-1]))) $str[$i]=$l;
			else if($i>0 && ($i<strlen5($str)-1) && $str[$i]=='"' && is_bukva(win_utf8($str[$i-1])) && !is_bukva(win_utf8($str[$i+1]))) $str[$i]=$r;
		}
	}
	$str=win_utf8($str);	
	return $str;
}

function convert_datetime($date,$time){
	$dd=explode('-',$date);
	$tt=explode(':',$time);
	return $dd[0]*12*31*24*60+$dd[1]*31*24*60+$dd[2]*24*60+$tt[0]*60+$tt[1];
}

function strtoupper5($str,$enc='UTF8'){
	if(function_exists('mb_strtoupper')) return mb_strtoupper($str,$enc);
	else return strtoupper($str);
}

function strtolower5($str,$enc='UTF8'){
	if(function_exists('mb_strtoupper')) return mb_strtolower($str,$enc);
	else return strtolower($str);
}

function ucfirst5($str){
	if(empty($str))return'';
	$str[0]=strtoupper5($str[0]);
	return $str;
}

function strlen5($str){
	return strlen($str);
	//return mb_strlen($str,'UTF-8');
}

function strpos5($str,$str2,$offset=-1){
	if($offset!=-1) return strpos($str,$str2,$offset); else return strpos($str,$str2);
	//if($offset!=-1) return mb_strpos($str,$str2,$offset,'UTF-8');
	//else  return mb_strpos($str,$str2,0,'UTF-8');
}

function strstr5($str,$str2){
	if(empty($str) || empty($str2)) return false;
	return strstr($str,$str2);
	//return mb_strstr($str,$str2,'UTF-8');
}

function substr5($str,$start,$length=-1){
	//if(gettype($length)=='string') debug_wayback();
	if($length!=-1) return substr($str,$start,$length); else return substr($str,$start);
	//if($length) return mb_substr($str,$start,$length,'UTF-8');
	//else return mb_substr($str,$start,false,'UTF-8');
}

function unhtmlentities($string) 
{
	$trans_tbl = get_html_translation_table(HTML_ENTITIES);
	$trans_tbl = array_flip($trans_tbl);
	return strtr($string, $trans_tbl);
}

function get_tag2($str,$t1,$t2){
	for($i=0;$i<strlen5($str);$i++) if (substr5($str,$i,strlen5($t1))==$t1){
		$pro_res='';		
		for($c=$i+strlen5($t1);$c<strlen5($str);$c++) if (substr5($str,$c,strlen5($t2))==$t2)
			return $pro_res;
		else $pro_res=$pro_res.$str[$c];
	}
	return '';
}
function get_all_tag3($str,$t1,$t2){
	$pro_res2=Array();
	$u=0;
	for($i=0;$i<strlen5($str);$i++) if (substr5($str,$i,strlen5($t1))==$t1){
		$pro_res='';		
		for($c=$i+strlen5($t1);$c<strlen5($str);$c++) if (substr5($str,$c,strlen5($t2))==$t2){
			$pro_res2[$u]=$pro_res;$u++;
			$i=$c+strlen5($t2)-1;
			break;
			//return $pro_res;
		}else $pro_res=$pro_res.$str[$c];
	}
	return $pro_res2;
}

function get_uid($val){
	return md5(serialize($val));
}

function check_filename($name){
	$allow2='~!()-_1234567890qwertyuiopasdfghjklzxcvbnm.[]@#$^&+= QWERTYUIOPASDFGHJKLZXCVBNM';
	for($i=0;$i<strlen($allow2);$i++) $allow[$allow2[$i]]=1;
	for($i=0;$i<strlen($name);$i++) if(empty($allow[$name[$i]])) return false;
	return true;
}

function basename2($name){
	return array_pop(explode('/',$name));
}

function uuid(){
	return md5(uniqid(rand(),1));
}
function uuin(){return uuid();}


function prepend_strposK($x,$m,&$kmp_next){
	$i=0; $j=-1;
	$kmp_next[0]=-1; 
	while ($i < $m){
		while($j>-1 && $x[$i]!=$x[$j]) $j=$kmp_next[$j];
		$i++;
		$j++;
		if ($x[$i]==$x[$j]) $kmp_next[$i]=$kmp_next[$j]; 
     		else $kmp_next[$i] = $j; 
	}
}

function strposK($x,$y,$pos=0) {
	$n=strlen($x);
	$m=strlen($y);
	$kmp_next=Array();
	$i=$pos; $j=0;
	prepend_strposK($y,$m,$kmp_next);
	while ($i<$n) {
		while ($j>-1 && $y[$j]!=$x[$i]) $j = $kmp_next[$j];
		$i++;
		$j++; 
		if ($j>=$m){
			return ($i-$j); 
			$j=$kmp_next[$j]; 
		}
	}
	return false;
}

function sbm_seek_code($data,$length){
	$r=Array();
	$result='';
	for($i=0;$i<$length-1;$i++)if(!isset($r[$i])){
		for($i2=$i+1;$i2<$length;$i2++){
			if($data[$i2]==$data[$i]){
				$r[$i2]=$i;
				$result.=$i.':'.$i2.'.';
			}
		}
	}
	return $result;
}

function seek_by_mask($data,$mask,$interval=1){//mask - искомый текст, data - зашифрованные данные: выдаёт массив совпадений; поиск осуществляется по совпадениям/несовпадениям букв в искомом и данных
	$length=strlen($data);
	$length_mask=strlen($mask);
	$mask_code=sbm_seek_code($mask,$length_mask);
	$result=Array();
	for($i=0;$i<$length-$length_mask;$i+=$interval){
		$text=substr($data,$i,$length_mask);
		$text_code=sbm_seek_code($text,$length_mask);
		if($text_code==$mask_code) $result[$i]=$text;
	}
	return $result;
}

function natksort(&$array) {
     $keys = array_keys($array);
     natcasesort($keys);

     foreach ($keys as $k) {
         $new_array[$k] = $array[$k];
     }

     $array = $new_array;
     return true;
 }
 
function dc($obj){
	if(is_object($obj)) return clone $obj;
	else return $obj;//unserialize(serialize($obj));
}

function ceiling($number, $significance = 1){ // © Scott Weaver
	return ( is_numeric($number) && is_numeric($significance) ) ? (ceil($number/$significance)*$significance) : false;
}
function flooring($number, $significance = 1){ // © Scott Weaver
	return ( is_numeric($number) && is_numeric($significance) ) ? (floor($number/$significance)*$significance) : false;
}
//	echo ceiling(0, 1000);     // 0
//	echo ceiling(1, 1);        // 1000
//	echo ceiling(1001, 1000);  // 2000
//	echo ceiling(1.27, 0.05);  // 1.30

function stripslashes_deep(&$value){
         $quotes_sybase = strtolower(ini_get('magic_quotes_sybase'));
         //$unescape_function = (empty($quotes_sybase) || $quotes_sybase === 'off') ? 'stripslashes($value)' : 'str_replace("\'\'","\'",$value)';
         if (is_string($value)) {
         	if(empty($quotes_sybase) || $quotes_sybase === 'off') $value=stripslashes($value);
         	else $value=str_replace("''","'",$value);
         } else if (is_array($value)) {
             foreach ($value as &$v) stripslashes_deep($v);
         }
}

function clean_magic_quotes(){
	if (function_exists('get_magic_quotes_gpc') && get_magic_quotes_gpc()) {
	     // Create lamba style unescaping function (for portability)  
	     /*
	     $quotes_sybase = strtolower(ini_get('magic_quotes_sybase'));
	     $unescape_function = (empty($quotes_sybase) || $quotes_sybase === 'off') ? 'stripslashes($value)' : 'str_replace("\'\'","\'",$value)';
	     $stripslashes_deep = create_function('&$value, $fn', '
	         if (is_string($value)) {
	             $value = ' . $unescape_function . ';
	         } else if (is_array($value)) {
	             foreach ($value as &$v) $fn($v, $fn);
	         }
	     ');
	    
	     // Unescape data
	     $stripslashes_deep($_POST, $stripslashes_deep);
	     $stripslashes_deep($_GET, $stripslashes_deep);
	     $stripslashes_deep($_COOKIE, $stripslashes_deep);
	     $stripslashes_deep($_REQUEST, $stripslashes_deep);
	     */
	     stripslashes_deep($_POST);
	     stripslashes_deep($_GET);
	     stripslashes_deep($_COOKIE);
	     stripslashes_deep($_REQUEST);
	}
}

function clean_magic_quotes_once(&$val){
	stripslashes_deep($val);
	return $val;
}

function add_magic_quotes_in(&$value){
	if(!empty($value)) foreach($value AS $var=>$v){
		if(is_array($v)) add_magic_quotes_in($value[$var]);
		else {
			$value[$var]=addslashes($value[$var]);
		}
	}
}

function add_magic_quotes(){
	add_magic_quotes_in($_POST);
	add_magic_quotes_in($_GET);
	add_magic_quotes_in($_COOKIE);
	add_magic_quotes_in($_REQUEST);
}

function safe_sql_input($v,$type=0){				
	//0 - simple
	//1 - for uncknown data in input
	//2 - for js/html entrance
	if($type==0){
		$v=str_replace("\\","\\\\",$v);
		$v=str_replace("\r","\\r",$v);
		$v=str_replace("\n","\\n",$v);
		$v=str_replace("'","''",$v);
		return $v;
	}
	if($type==1){
		//global $db;
		//return $db->escape($v);\
		$v=(string)$v;
		if(empty($v)) return $v;
		$a=strlen($v);
		$p='';
		$s=false;
		$r='';
		for($i=0;$i<$a;$i++){
			$b=$v[$i];
			if($i<$a-1) $n=$v[$i+1]; else $n='';
			if($b=="'"){
				if($p=="'" || $s){
					$r.=$b;
					$p='';
					$s=false;
					continue;
				} else if($n!="'") $r.="'";
			}
			$r.=$b;
			if($b=="\\" && !$s) $s=true; else $s=false;
			if($s) $p=''; else $p=$v[$i];
		}
		return $r;
	}
	if($type==2){
		$v=str_replace("'","`",$v);
		$v=str_replace('"','``',$v);
		$v=str_replace(">"," ",$v);
		$v=str_replace("<","",$v);
		$v=str_replace('<option ','',$v);
		return $v;
	}
}

if(!function_exists('mb_substr')){ function mb_substr($string,$start,$length=NULL,$encoding=''){ return substr($string,$start,$length);}}
if(!function_exists('mb_strlen')){function mb_strlen($string,$encoding=''){	return strlen($string); }}
if(!function_exists('mb_strtoupper')){function mb_strtoupper($string,$encoding=''){	return strtoupper($string); }}
if(!function_exists('mb_strtolower')){function mb_strtolower($string,$encoding=''){	return strtolower($string); }}

function start_buffer(){
	$tmp=ob_get_contents();
	if($tmp) ob_end_clean();
	ob_start();
	return $tmp;
}

function return_buffer($tmp){
	$res=ob_get_contents();
	ob_end_clean();
	if($tmp){
		ob_start();
		echo $tmp;
	}
	error_reporting(REPORTING_LEVEL);
	return $res;
}

function preload_lng(){
	global $lng, $preload_lng, $LNG_PATHS, $local_lng, $lngp;
	if(!empty($preload_lng)) return false;
	$preload_lng=1;
	$fls=Array();
	foreach($LNG_PATHS as $path) $fls=find_file($path.'/'.$local_lng.'/',$fls,1);
	$tmp='';
	if(!empty($fls)) foreach($fls AS $f){
		$tmp.=VSP2.'[GLOBALS]'.VSP2.file_get_contents($f->url);
	}
	$tmp=explode(VSP2,$tmp);
	if(!empty($tmp)) foreach($tmp AS $line){
		$x=strlen($line);
		if($x>2 && $line[0]=='[' && $line[$x-1]==']'){
			$cur_part=substr($line,1,$x-2);
			if($cur_part=='GLOBALS') $cur_part='';
		}
		$line=explode('=',$line);
		if(count($line)<2) continue;
		$lng[$line[0]]=$line[1];
		if(!empty($cur_part)) $lngp[$cur_part][$line[0]]=$line[1];
	}
}

function lng($text){
	global $lng, $preload_lng, $LNG_PATHS, $lng_part, $lngp;
	//preloading lng
	if(empty($lng) && empty($preload_lng)) preload_lng();
	if(!empty($lng_part) && isset($lngp[$lng_part]) && isset($lngp[$lng_part][$text])) return $lngp[$lng_part][$text];
	if(isset($lng[$text])) return $lng[$text]; else return $text;
}

function define_lng($set_lng_part=''){
	global $lng_part;
	if(!empty($lng_part)) $res=$lng_part; else $res='';
	$lng_part=$set_lng_part;
	return $res;
}

$log_msgs=Array();
function add_log_msg($str){
	global $log_msgs;
	$log_msgs[]=$str;
}

function alog($str){
	add_log_msg($str);
}


// DOM PARSER
$dom_index=0;
$dom_length=0;
$dom_data='';
$single_tags=Array();
$html_spaces=Array();
define('DOM_STRING',0);
define('DOM_SINGLE_TAG',1);
define('DOM_TAG',2);

function build_next_dom($wait_for=''){
	global $dom_index, $dom_data, $dom_length, $single_tags,$html_spaces,$except_deep_tags;
	$res=Array();
	$prev_space=false;
	while($dom_index<$dom_length){
		$s=$dom_data[$dom_index];
		if($dom_index+1<$dom_length) $s2=$dom_data[$dom_index+1]; else $s2='';
		$dom_index++;		
		if($s!='<' || $s2=='!'){
			if(isset($html_spaces[$s])){
				if(ord($s)==10 && !empty($s2) && ord($s2)=='13') $dom_index++;
				if((ord($s)==10 || ord($s)==13) && (empty($cdata) || empty($cdata->data))){
					$prev_space=true;
					continue;
				}
				if(!$prev_space){
					$prev_space=true;
					$s=' ';
				} else continue;
			} else $prev_space=false;
			if(empty($cdata)){
				$cres=count($res);
				$res[$cres]->type=DOM_STRING;
				$res[$cres]->data='';	
				$cdata=&$res[$cres];
			}
			$cdata->data.=$s;
		}		
		if($s=='<' && $s2=='/'){
			$dom_index++;
			$name='';
			$first=true;
			while($dom_index<$dom_length){
				if(!$first) $dom_index++;
				$first=false;
				if($dom_data[$dom_index]=='>') break;
				$name.=$dom_data[$dom_index];
			}
			if($name==$wait_for){
				$dom_index++;
				return $res;
			} else {
				if(empty($cdata)){
					$cres=count($res);
					$res[$cres]->type=DOM_STRING;
					$res[$cres]->data='';	
					$cdata=&$res[$cres];
				}
				$cdata->data.='</'.$name.'>';
				$dom_index++;
			}
		}		
		if($s=='<' && $s2!='/' && $s2!='!'){
			$cres=count($res);
			$res[$cres]->type=DOM_TAG;
			$res[$cres]->name='';
			$res[$cres]->prop=Array();
			$cdata=&$res[$cres];
			$is_string=false;//"
			$is_string2=false;//'
			$name='';		
			$first=true;
			while($dom_index<$dom_length){
				if(!$first) $dom_index++;
				$first=false;
				if(!$is_string && !$is_string2 && $dom_data[$dom_index]=='"'){
					$is_string=true;
					$name.='"';
					continue;
				}
				if(!$is_string && !$is_string2 && $dom_data[$dom_index]=="'"){
					$is_string2=true;
					$name.="'";
					continue;
				}
				if($is_string && !$is_string2 && $dom_data[$dom_index]=='"'){
					$is_string=false;
					$name.='"';
					continue;
				}
				if($is_string && !$is_string2 && $dom_data[$dom_index]=="'"){
					$name.="'";
					continue;
				}
				if(!$is_string && $is_string2 && $dom_data[$dom_index]=="'"){
					$is_string2=false;
					$name.="'";
					continue;
				}
				if(!$is_string && $is_string2 && $dom_data[$dom_index]=='"'){
					$name.="'";
					continue;
				}						
				if($is_string || $is_string2){
					$name.=$dom_data[$dom_index];
					continue;
				}
				if(!$is_string && !$is_string2 && $dom_data[$dom_index]!=' ' && $dom_data[$dom_index]!='>'){
					$name.=$dom_data[$dom_index];
					continue;
				}
				if(!$is_string && !$is_string2 && ($dom_data[$dom_index]==' ' || $dom_data[$dom_index]=='>')){
					if(empty($cdata->name)){
						$cdata->name=$name;
						$name='';
					} else {
						$cdata->prop[]=$name;
						$name='';
					}
					if($dom_data[$dom_index]=='>'){
						break;
					}
				}
			}
			if(!empty($name)){
				if(empty($cdata->name)){
					$cdata->name=$name;
				} else $cdata->prop[]=$name;
			}
			if(!empty($cdata->name)){
				if(isset($single_tags[strtolower($cdata->name)])){
					$cdata->type=DOM_SINGLE_TAG;
					$dom_index++;
				} else if(isset($except_deep_tags[strtolower($cdata->name)])){
					$cdata->type=DOM_SINGLE_TAG;
					$dom_index++;
				} else {
					$dom_index++;
					$cdata->sub=build_next_dom($cdata->name);
				}
			}
			unset($cdata);
			continue;
		}
	}
	return $res;
}

function build_dom_tree($data){
	global $dom_index, $dom_data, $dom_length, $single_tags, $html_spaces, $except_deep_tags;
	$dom_index=0;
	$dom_data=$data;
	$dom_length=strlen($data);
	$single_tags=array_flip(explode(',','area,base,basefont,bgsound,br,col,command,embed,hr,img,input,isindex,keygen,link,meta,param,source,track,wbr,?xml,enclosure'));
	$html_spaces=array_flip(Array(chr('10'),chr('13'),' ','	'));	
	$except_deep_tags=array_flip(Array('html'));
	return build_next_dom();
}

function only_spaces($str){
	if(empty($str)) return true;
	$html_spaces=array_flip(Array(chr('10'),chr('13'),' ','	'));
	for($i=0;$i<strlen($str);$i++) if(!isset($html_spaces[$str[$i]])) return false;
	return true;
}

function cascading_html_recursive($rows,$step=0,$gfirst=false){
	if(empty($rows)) return '';
	$res='';
	$tab='';
	for($i=0;$i<$step;$i++) $tab.='	';
	$t=chr(10).$tab;
	
	//$t=chr(10).$step.':'.$tab;
	
	// DEBUGGING
	/*$res.=$t;
	foreach($rows AS $row){
		if($row->type==DOM_STRING) $res.=$t.$row->data;
		else {
			$res.=$t.$row->name;
			$res.=cascading_html_recursive($row->sub,$step+1);
		}
	}
	return $res;*/
	
	$first=true;
	foreach($rows AS $row){
		//$t=chr(10).$tab;//chr(10).$step.':'.$row->name.':'.$tab;
		//if(!$first) $res.=chr(10).$tab;
		$first=false;
		if($row->type==DOM_STRING && !empty($row->data)) $res.=($gfirst?'':$t).$row->data;
		if($row->type==DOM_SINGLE_TAG || $row->type==DOM_TAG){
			$res.=($gfirst?'':$t).'<'.$row->name;
			if(!empty($row->prop)) $res.=' '.implode(' ',$row->prop);
			$res.='>';
		}
		if($row->type==DOM_TAG){
			if(!empty($row->sub)){
				//$res.=chr(10);
				//if(count($row->sub)>1 || $row->sub[0]->type!=DOM_STRING || !only_spaces($row->sub[0]->data))
				if(count($row->sub)==1 && $row->sub[0]->type==DOM_STRING){
					$res.=$row->sub[0]->data;
				} else {
					$res.=cascading_html_recursive($row->sub,$step+1);
					//$res.=chr(10).$tab;
					$res.=$t;
				}
			}			
			$res.='</'.$row->name.'>';
		}
		$gfirst=false;
	}
	return $res;
}

function cascading_html($str,$step=0,$gfirst=true){
	return cascading_html_recursive(build_dom_tree($str),$step,$gfirst);
}

function get_form_protection_key($from='',$use_session=false,$return_input=false,$key_name='key'){
	global $user,$e5uid,$protect_form_by_session;
	if(!$protect_form_by_session) $use_session=false;
	if($use_session && isset($user->session)) $session=$user->session.';'.$user->ip;
	else $session=$user->ip;	
	$key=md5($session.$e5uid.$from);
	if($return_input) return '<input type="hidden" name="'.$key_name.'" value="'.$key.'">';
	else return $key;
}

function check_form_protection_key($key,$from=Array(),$use_session=false){
	global $user,$e5uid,$protect_form_by_session;	
	if(!$protect_form_by_session) $use_session=false;
	if(!empty($from) && !is_array($from)) $from=Array($from);
	if(!empty($from)){
		$tmp=$_SERVER['HTTP_REFERER'];
		if(empty($tmp)) return false;
		if(strstr($tmp,'?')){
			$tmp=explode('?',$tmp);
			$tmp=$tmp[0];
		}
		if(strstr($tmp,'#')){
			$tmp=explode('#',$tmp);
			$tmp=$tmp[0];
		}
		if($tmp[strlen($tmp)-1]=='/') $tmp=substr($tmp,0,strlen($tmp)-1);
		$tmp=explode('/',$tmp);
		$real_from=$tmp[count($tmp)-1];
		$valid_from=false;
		foreach($from AS $f) if($f==$real_from){
			$valid_from=true;
			break;
		}
		if(!$valid_from) return false;
	}
	if($use_session && isset($user->session)){
		$session=$user->session.';'.$user->ip;
	} else $session=$user->ip;
	if(empty($from)) return ($key==md5($session.$e5uid));
	else foreach($from AS $f) if($key==md5($session.$e5uid.$f)) return true;
	return false;
}

?>