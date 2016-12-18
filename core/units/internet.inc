<?php

function simple_mail($from,$to,$topic,$content,$charset='utf-8'){
	$mailheaders="Content-type: text/html; charset=".$charset."\r\n";
	$mailheaders.="From: ".$from."\n";
	$mailheaders.="Reply-To: $from\n";
	mail($to,$topic,$content,$mailheaders);
}

function in_ip($min,$max,$ip){
	$min=explode('.',$min);
	$max=explode('.',$max);
	$ip=explode('.',$ip);
	if($ip[0]<$min[0] || $ip[0]>$max[0]) return false;
	if($ip[1]<$min[1] || $ip[1]>$max[1]) return false;
	if($ip[2]<$min[2] || $ip[2]>$max[2]) return false;
	if($ip[3]<$min[3] || $ip[3]>$max[3]) return false;
	return true;
}

function get_country(){
	global $user;
	if(!empty($_COOKIE["user_whois"]) && empty($user->country)){
		$tmp=explode('_',$_COOKIE["user_whois"]);
		if(md5($tmp[0].$tmp[1])!=$tmp[2]) $_COOKIE["user_whois"]='';
		else {
			$user->town=$tmp[0];
			$user->country=$tmp[1];
		}
	}
	if(empty($_COOKIE["user_whois"]) && empty($user->country) && empty($is_offline)){
		/*$vars["action"]='checkip_whois';
		$vars["ip"]=$user->ip;
		$vars["query"]=$user->ip;
		$data=do_post('www.ip2city.ru/index.php',$vars);
		$user->town=strtolower(get_tag($data,utf8_win('Город: '),'<br>'));
		$user->country=strtolower(get_tag($data,utf8_win('Страна: '),'<br>'));*/
		
		//$user->ip='212.164.230.75';
		// этот метод работает, но определяет с малой точностью
		//$data=file_get_contents('http://www.geobytes.com/IpLocator.htm?GetLocation&IpAddress='.$user->ip.'&template=php3.txt');
		//$user->town=strtolower(get_tag($data,'city" content="','"'));
		//$user->country=strtolower(get_tag($data,'country" content="','"'));
		
		$data=file_get_contents('http://gd.geobytes.com/GetCityDetails?callback=callback&fqcn='.$user->ip);
		$user->country=strtolower(get_tag($data,'"geobytescountry":"','"'));
		$user->town=strtolower(get_tag($data,'"geobytescity":"','"'));
		
		//file_get_contents("http://api.hostip.info/get_json.php?ip=".$user->ip);
		//file_get_contents('http://resident.su/ipgeo/?query='.$user->ip);
		SetCookie("user_whois",$user->town.'_'.$user->country.'_'.md5($user->town.$user->country),time()+302400);
	}
}

function check_ip($ip1,$ip2){
	if(empty($ip1)) return true;
	if(strpos5($ip1,'+')){
		$ip1=explode('+',$ip1);
		foreach($ip1 AS $ip) if(check_ip($ip,$ip2)) return true;
	} else {
		if(strstr($ip1,',')) $ip1b=explode(',',$ip1); else {$ip1b=Array(); $ip1b[]=$ip1;}
		foreach($ip1b AS $ip1){
			$invert=false;
			if($ip1[0]=='!'){
				$ip1=substr5($ip1,1,strlen5($ip1)-1);
				$invert=true;
			}
			if(!empty($ip1) && $ip1[0]=='"' && !empty($_SERVER["HTTP_USER_AGENT"])){
				$browser=substr5($ip1,1,strlen5($ip1)-2);
				$t=strstr(strtolower($_SERVER["HTTP_USER_AGENT"]),strtolower($browser));
				if(!$invert) $res=$t;
				else $res=!$t;
				if($res) $GLOBALS["zone_specify"]=strtolower($browser);
				if(!$res) return false; else continue;
			}
			if(!empty($ip1) && $ip1[0]=="'" && !empty($_SERVER["HTTP_REFERER"])){
				$referer=substr5($ip1,1,strlen5($ip1)-2);
				$t=strstr(strtolower(urldecode($_SERVER["HTTP_REFERER"])),strtolower($referer));
				if(!$invert) $res=$t;
				else $res=!$t;
				if($res) $GLOBALS["zone_specify"]=strtolower($referer);
				if(!$res) return false; else continue;
			}
			if(strpos5($ip1,'-')){
				$range=explode('-',$ip1);
				if(is_int(str_replace('.','',$range[0])) && is_int(str_replace('.','',$range[1]))){
					$t=in_ip($range[0],$range[1],$ip2);
					if(!$invert) $res=$t;
					else $res=!$t;
					if(!$res) return false; else continue;
				}
			}
			if(strpos5($ip1,'.') || $ip1=='*'){
				$t=($ip1==$ip2 || $ip1=='*');
				if(!$invert) $res=$t;
				else $res=!$t;
				if(!$res) return false; else continue;
			}
			global $user;
			//$data=do_get('http://www.whois-search.com/whois/'.$user->ip);
			get_country();
			$ip1=strtolower($ip1);
			$t= $ip1==$user->town || $ip1==$user->country;
			if(!$invert) $res=$t;
			else $res=!$t;
			if(!$res) return false; else continue;
		}
		return true;
	}
}

function curl_get($url,$get_header=1){
	//if(!function_exists('curl_init')) return false;
	$curl = curl_init();
	curl_setopt($curl, CURLOPT_URL, $url);
	curl_setopt($curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_setopt($curl, CURLOPT_HEADER, $get_header);
	curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($curl, CURLOPT_USERAGENT, 'Opera/9.80 (Windows NT 5.1; U; ru) Presto/2.7.62 Version/11.01');
	$res = curl_exec($curl);
	curl_close($curl);
	return $res;
}

function do_get($url,$cookies='',$referer='',$browser='Mozilla/5.0 (Windows NT 5.2; WOW64; rv:2.0) Gecko/20100101 Firefox/4.0',$http='1.0',$charset='utf-8'){
	$url=explode('/',str_replace('Http://','',str_replace('HTTP://','',str_replace('http://','',$url))));
	$host=$url[0];
	$tmp='';
	for($i=1;$i<count($url);$i++){$tmp.='/'; $tmp.=$url[$i];}
	$url=$tmp;
	if(!empty($cookies)) $cookies='
Cookie: '.$cookies;
	if(!empty($referer)) $referer='
Referer: '.$referer;
	$s='GET '.$tmp.' HTTP/'.$http.'
Host: '.$host.'
User-Agent: '.$browser.'
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: ru-ru,ru;q=0.8,en-us;q=0.5,en;q=0.3
Accept-Encoding: deflate
Accept-Charset: '.$charset.';q=0.7,*;q=0.7'.$cookies.$referer;
	$s=str_replace('
',"\r\n",$s);
	$s.="\r\n\r\n\r\n";
	$fs=fsockopen($host,80,$errno,$errstr,10);
	$res='';
	if(!$fs) return '';
	fputs($fs,$s);
	while (!feof($fs)) {
		$tmp=fgets($fs);
		if(empty($tmp)) break;
		$res.=$tmp;
	}
	fclose($fs);
	return $res;
}

function do_post($url,$vars=Array(),$cookies='',$referer='',$browser='Mozilla/5.0 (Windows NT 5.2; WOW64; rv:2.0) Gecko/20100101 Firefox/4.0',$http='1.0',$charset='windows-1251,utf-8'){
	$post='';
	foreach($vars AS $var=>$value){
		if(!empty($post)) $post.='&';
		$post.=$var.'='.urlencode($value);
	}
	$url=explode('/',str_replace('Http://','',str_replace('HTTP://','',str_replace('http://','',$url))));
	$host=$url[0];
	$tmp='';
	for($i=1;$i<count($url);$i++){$tmp.='/'; $tmp.=$url[$i];}
	$url=$tmp;
	if(!empty($cookies)) $cookies='
Cookie: '.$cookies;
	if(!empty($referer)) $referer='
Referer: '.$referer;
	$s='POST '.$tmp.' HTTP/'.$http.'
Host: '.$host.'
User-Agent: '.$browser.'
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: ru-ru,ru;q=0.8,en-us;q=0.5,en;q=0.3
Accept-Encoding: 
Accept-Charset: '.$charset.';q=0.7,*;q=0.7'.$cookies.$referer.'
Content-Type: application/x-www-form-urlencoded
Content-Length: '.strlen($post).'
Connection: Close
';
	$s=str_replace('
',"\r\n",$s);
	$s.="\r\n";
	$s.=$post;
	$s.="\r\n\r\n";
	@$fs=fsockopen($host,80,$errno,$errstr,10);
	$res='';
	if(!$fs) return '';
	fputs($fs,$s);
	while (!feof($fs)) {
		$tmp=fgets($fs);
		if(empty($tmp)) break;
		$res.=$tmp;
	}
	fclose($fs);
	return $res;
}
 
 function GetUserAgent() { 
  $str=$_SERVER["HTTP_USER_AGENT"]; 
  if (strpos($str,"Chrome")!==false) return "chrome"; 
  elseif (strpos($str,"Opera")!==false) return "opera"; 
  elseif (strpos($str,"SeaMonkey")!==false) return "mozilla"; 
  elseif (strpos($str,"FireFox")!==false) return "mozilla"; 
  elseif (strpos($str,"Safari")!==false) return "safari"; 
  elseif (strpos($str,"MSIE")!==false) return "is"; 
  return "else"; 
}

function antixss($html){
	//autor: kzpromo
	$find = array ('/data:/i', '/about:/i', '/vbscript:/i', '/onscroll/i', '/onclick/i', '/onload/i', '/onunload/i', '/onabort/i', '/onerror/i', '/onblur/i', '/onchange/i', '/onfocus/i', '/onreset/i', '/onsubmit/i', '/ondblclick/i', '/onkeydown/i', '/onkeypress/i', '/onkeyup/i', '/onmousedown/i', '/onmouseup/i', '/onmouseover/i', '/onmouseout/i', '/onselect/i', '/javascript/i', '/javascript/i' );
	$replace = array ("d&#097;ta:", "&#097;bout:", "vbscript<b></b>:", "&#111;nscroll", "&#111;nclick", "&#111;nload", "&#111;nunload", "&#111;nabort", "&#111;nerror", "&#111;nblur", "&#111;nchange", "&#111;nfocus", "&#111;nreset", "&#111;nsubmit", "&#111;ndblclick", "&#111;nkeydown", "&#111;nkeypress", "&#111;nkeyup", "&#111;nmousedown", "&#111;nmouseup", "&#111;nmouseover", "&#111;nmouseout", "&#111;nselect", "j&#097;vascript" );
	
	$html = preg_replace( $find, $replace, $html );
	$html = preg_replace( "#<iframe#i", "&lt;iframe", $html );
	$html = preg_replace( "#iframe>#i", "iframe&gt;", $html );
	$html = preg_replace( "#<script#i", "&lt;script", $html );
	$html = preg_replace( "#script>#i", "script&gt;", $html );
	$html = preg_replace( "#</script#i", "&lt;/script", $html );
	$html = str_replace( "<?", "&lt;?", $html );
	$html = str_replace( "?>", "?&gt;", $html );
	//$html = str_replace( '">', '`&gt;', $html );
	//$html = str_replace( "'>", "`&gt;", $html );
	return $html;
}

function array_anti_xss($array){
	//autor: kzpromo
	if(is_array($array) && count($array)){
		foreach($array as $k => $v) $array[$k] = array_anti_xss($v);
		return $array;
	}else return antixss_value($array);
}

function check_xss($type=0){//0 - only XSS, 1 - only SQLi, 2 - XSS+SQLi
	if($type==0 || $type==2){
		if($_POST) $_POST = array_anti_xss($_POST);
		if($_GET) $_GET = array_anti_xss($_GET);
		if($_COOKIE) $_COOKIE = array_anti_xss($_COOKIE);
		if($_FILES) $_FILES = array_anti_xss($_FILES);
	}
	if($type==1 || $type==2){
		$bag_req = array("select", "eval", "EXEC", "UPDATE", "LIMIT", "INSERT", "INTO", "union", "CONCAT", "INFORMATION_SCHEMA", "OUTFILE", "DUMPFILE", "LOAD_FILE", "BENCHMARK", "SUBSTRING", "ASCII", "CHAR", "database", "HEX", "\\.\\/", "%00", "\\.htaccess", "config\\.php", "document\\.cookie", "'");
		if($_SERVER['REQUEST_METHOD']=="GET"){
			$request = serialize($_GET);
		} else  {
			$request = serialize($_POST);
		}
		$urequest = urldecode($request);
		$brequest = base64_decode($request);
		if($_GET || $_POST) foreach ($bag_req as $key => $value) {
			if(preg_match("/$value/i", $request) || preg_match("/$value/i", $urequest) || preg_match("/$value/i", $brequest)){
				header("Location: /");
				die();
			}
		}
	}
}

// Copyright (c) 2001-2008 Liip AG. Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0. Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
// Author: Christian Stocker <christian.stocker@liip.ch>
// Modification for CMS E5
function antixss_value($string) {
        //$string = str_replace(array("&amp;","&lt;","&gt;"),array("&amp;amp;","&amp;lt;","&amp;gt;"),$string);
        // fix &entitiy\n;
        //$string = preg_replace('#(&\#*\w+)[\x00-\x20]+;#u',"$1;",$string);
        //$string = preg_replace('#(&\#x*)([0-9A-F]+);*#iu',"$1$2;",$string);
        //$string = html_entity_decode($string, ENT_COMPAT, "UTF-8");
        
        // remove any attribute starting with "on" or xmlns
        $string = preg_replace('#(<[^>]+[\x00-\x20\"\'\/])(on|xmlns)[^>]*>#iUu', "$1>", $string);
        
        // remove javascript: and vbscript: protocol
        $string = preg_replace('#([a-z]*)[\x00-\x20\/]*=[\x00-\x20\/]*([\`\'\"]*)[\x00-\x20\/]*j[\x00-\x20]*a[\x00-\x20]*v[\x00-\x20]*a[\x00-\x20]*s[\x00-\x20]*c[\x00-\x20]*r[\x00-\x20]*i[\x00-\x20]*p[\x00-\x20]*t[\x00-\x20]*:#iUu', '$1=$2nojavascript...', $string);
        $string = preg_replace('#([a-z]*)[\x00-\x20\/]*=[\x00-\x20\/]*([\`\'\"]*)[\x00-\x20\/]*v[\x00-\x20]*b[\x00-\x20]*s[\x00-\x20]*c[\x00-\x20]*r[\x00-\x20]*i[\x00-\x20]*p[\x00-\x20]*t[\x00-\x20]*:#iUu', '$1=$2novbscript...', $string);
        $string = preg_replace('#([a-z]*)[\x00-\x20\/]*=[\x00-\x20\/]*([\`\'\"]*)[\x00-\x20\/]*-moz-binding[\x00-\x20]*:#Uu', '$1=$2nomozbinding...', $string);
        $string = preg_replace('#([a-z]*)[\x00-\x20\/]*=[\x00-\x20\/]*([\`\'\"]*)[\x00-\x20\/]*data[\x00-\x20]*:#Uu', '$1=$2nodata...', $string);
        
        //remove any style attributes, IE allows too much stupid things in them, eg.
        //<span style="width: expression(alert('Ping!'));"></span> 
        // and in general you really don't want style declarations in your UGC
        //$string = preg_replace('#(<[^>]+[\x00-\x20\"\'\/])style[^>]*>#iUu', "$1>", $string);
        
        //remove namespaced elements (we do not need them...)
        $string = preg_replace('#</*\w+:\w[^>]*>#i',"",$string);
        //remove really unwanted tags        
        do {
            $oldstring = $string;
            $string = preg_replace('#</*(applet|meta|xml|blink|link|script|embed|object|iframe|frame|frameset|ilayer|layer|bgsound|title|base)[^>]*>#i',"",$string);
        } while ($oldstring != $string);
        
        return $string;
}

function unset_cookie($cookie){
	SetCookie($cookie,"",time()-3600 , '/' , '' , 0, 1/*, 1*/ );	
	//SetCookie($cookie,"",0 , '/' , '' , 0 , 1 );	
}


?>