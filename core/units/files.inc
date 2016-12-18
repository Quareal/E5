<?php

if(!empty($local_region) && file_exists(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/files.inc')) include_once(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/files.inc');

function find_file($dir,$res=Array(),$subdir=0){
	if(!file_exists(DOCUMENT_ROOT.'/'.$dir)) return $res;
	if ($handle = opendir(DOCUMENT_ROOT.'/'.$dir)) {
		while (false !== ($file = readdir($handle))) { 
			if($file!='.' && $file!='..' && empty($GLOBALS["DISALLOW_PATHS"][$dir.$file]) && is_dir(DOCUMENT_ROOT.'/'.$dir.$file) && $file!='include' && check_filename($file)){
				$res[$dir.$file]->url=$dir.$file;
				$res[$dir.$file]->size='-';
				$res[$dir.$file]->name='';
				if($subdir) $res=find_file($dir.$file.'/',$res,1);
			}
			if($file!='.' && $file!='..' && empty($GLOBALS["DISALLOW_PATHS"][$dir.$file]) && empty($GLOBALS["DISALLOW_FILES"][$dir.$file]) && !is_dir(DOCUMENT_ROOT.'/'.$dir.$file) && $file!='config.inc' && $file!='version.inc' && $file!='timing.inc' && $file!='cache' && $file!='include' && $file!='domain.php' && check_filename($file)){
				$res[$dir.$file]->url=$dir.$file;
				$res[$dir.$file]->name=$dir.$file;
				$res[$dir.$file]->size=filesize(DOCUMENT_ROOT.'/'.$dir.$file);
			}
		}
		closedir($handle); 
	}
	return $res;
}

$dir_access=0;
function get_files($dir,$glob_param=0){
	global $dir_access;
	if($dir_access==0) return glob($dir,$glob_param);
	else {
		$res=Array();
		if ($handle = opendir($dir)) {	
			while (false !== ($entry = readdir($handle))) $res[]=$entry;
			closedir($handle);
		}
		return $res;
	}
}

function scan_dir($dir,$files=Array(),$subdir=1,$only_dir=0,$inst=0,$ignore_check=0,$max_step=0,$max_res_in_fldr=0,$folder_var=0,$step=0)
// only dir = 1 - только папки, 2 - только файлы, 0 - и то и другое
// inst - параметры, необходимые только для сборки инсталляционного пакета (включает .htaccess)
// ignore_check - убирает проверку на валидность имени файла (только англ.)
// max_step - максмальный уровень вложенности (0 - без ограничений)
// max_res_in_fldr - максимальное кол-во результатов для одной папки, не считая корня (например, не больше 10) (0 - без ограничений)
// folder_var - что будет являться индексом массива: 0 - номер файла, 			1 - папка
//										       0 - $files[$i]=$dir.$filename,  	1 - $files[$dir][$i]=$dir.$filename;  (/*dir проходит очистку от DOCUMENT_ROOT*/)
{
    if($inst) $sp="/{,.}*"; else $sp="/*";
    if($only_dir==0 && $step!=0){
	if(!$folder_var) $files[]=$dir;
    }
    if ($objs = get_files($dir.$sp,GLOB_BRACE)) {
        if($max_res_in_fldr!=0 && count($objs)>$max_res_in_fldr && $step>0){
		$objs=array_slice($objs,0,$max_res_in_fldr);
        }
        foreach($objs as $obj)if(($ignore_check || check_filename(basename2($obj)))/* && str_replace($dir,'',$obj)!='.'*/ && str_replace($dir,'',$obj)!='/.' && str_replace($dir,'',$obj)!='/..') {
           //echo $obj.' - '.is_dir($obj).'<br>';
           if(is_dir($obj) && $subdir && ($max_step==0 || $max_step>$step)){
           	if($folder_var) $files[$dir][]=$obj;
           	else $files[]=$obj;
		$files=scan_dir($obj,$files,$subdir,$only_dir,$inst,$ignore_check,$max_step,$max_res_in_fldr,$folder_var,$step+1);
           } else {
		if((is_dir($obj) && $only_dir==1) || ($only_dir==2 && !is_dir($obj)) || $only_dir==0){
			//$sdir=str_replace(DOCUMENT_ROOT.'/','',$dir);;
			if($folder_var) $files[$dir][]=$obj;
			else $files[]=$obj;
		}
           }
        }
    }
    return $files;
}

function scan_size($dir,$dirs=Array()){
    $sp="/*";
    if ($objs = get_files($dir.$sp,GLOB_BRACE)) {
        foreach($objs as $obj)if(str_replace($dir,'',$obj)!='/.' && str_replace($dir,'',$obj)!='/..') {
           if(is_dir($obj)){
		$dirs=scan_size($obj,$dirs);
           } else {
		if(empty($dirs[dirname($obj)])) $dirs[dirname($obj)]=0;
		$dirs[dirname($obj)]+=filesize($obj);
           }
        }
    }
    return $dirs;
}

function echo_sizes($start_dir){
	$x=scan_size($start_dir);	
	asort($x);		
	foreach($x AS $name=>$size){
	   echo smart_size($size).' = '.$name.'<br>';
	}
}

function filename($url){
	if(!strstr($url,'/')) return $url;
	if($url[strlen($url)-1]=='/') $url=substr($url,0,strlen($url)-1);
	$url=explode('/',$url);
	return $url[count($url)-1];
}

function removeDirRec($dir)
{
    if ($objs = get_files($dir."/*")) {
        foreach($objs as $obj) {
            is_dir($obj) ? removeDirRec($obj) : unlink($obj);
        }
    }
     if(file_exists($dir)) @rmdir($dir);
}

function sizeDirRec($dir)
{
    $size=0;
    if ($objs = get_files($dir."/*",GLOB_BRACE)) {
        foreach($objs as $obj) {
            is_dir($obj) ? $size+=sizeDirRec($obj) : $size+=filesize($obj);
        }
    }
    return $size;
}


function rmdir2($dir){
	removeDirRec(rtrim($dir,'/.'));
}

function deltemp(){
	//template and misc cache
	removeDirRec(rtrim(FTEMP,'/.'));
	if(!file_exists(FTEMP)) mkdir(FTEMP);
	chmod(FTEMP,DEF_DRMOD);
	//json objects
	removeDirRec(rtrim(JTEMP,'/.'));
	if(!file_exists(JTEMP)) mkdir(JTEMP);
	chmod(JTEMP,DEF_DRMOD);
}

function reset_module_json($id=0,$sname='',$uin=''){
	if(empty($sname) && !empty($id)) $sname=getrowval("SELECT * FROM main_module WHERE module_id=$id","module_sname");
	if(empty($sname) && !empty($uid)) $sname=getrowval("SELECT * FROM main_module WHERE module_uin='$uin'","module_sname");
	if(file_exists(JTEMP.$sname.'.json')) unlink(JTEMP.$sname.'.json');
}

function reset_components_json(){
	$sname='component';
	if(file_exists(JTEMP.$sname.'.json')) unlink(JTEMP.$sname.'.json');
}

function get_ext($file){
	if(strpos($file,'.')){
		$ext=explode('.',$file);
		$ext='.'.$ext[count($ext)-1];
	} else $ext='';
	return $ext;
}

function get_ext2($file){
	if(strpos($file,'.')){
		$ext=explode('.',$file);
		$ext=$ext[count($ext)-1];
	} else $ext='';
	return $ext;
}

function clean_filename($filename){
	$filename=basename($filename);
	if(strstr($filename,'.')){
		$filename2=explode('.',$filename);
		$filename='';
		for($i=0;$i<count($filename2)-1;$i++){
			if($i>0) $filename.='.';
			$filename.=$filename2[$i];
		}		
	}
	$filename=latinica($filename);
	$chars=explode('~','Q~W~E~R~T~Y~U~I~O~P~A~S~D~F~G~H~J~K~L~Z~X~C~V~B~N~M~1~2~3~4~5~6~7~8~9~0~-~_~q~w~e~r~t~y~u~i~o~p~a~s~d~f~g~h~j~k~l~z~x~c~v~b~n~m~.~,~ ');
	$chars=array_flip($chars);
	$result='';
	for($i=0;$i<strlen($filename);$i++) if(isset($chars[$filename[$i]])) $result.=$filename[$i];
	return $result;
}

function find_name($folder,$prefix,$ext='',$compare_db=0,$genname=0,$filename=''){//compare_db=1 исключает попадание нового файла в строки БД
	if($genname==0) $i=1;
	if($genname==1) $i=uuin();
	if($genname==2) $i=clean_filename($filename);
	if(empty($ext) && !empty($filename)){
		$ext=explode('.',$filename);
		$ext=$ext[count($ext)-1];
	}
	if($prefix!='' && strlen($i)>=$prefix && substr($i,0,strlen($prefix))==$prefix) $prefix='';
	$ok=false;
	$z=1;
	while(!$ok){
		while(file_exists(DOCUMENT_ROOT.$folder.$prefix.$i.$ext)){
			if($genname==0) $i++;
			if($genname==1) $i=uuin();
			if($genname==2){
				$i=clean_filename($filename).$z;
				$z++;
			}
		}
		if(!$compare_db) $ok=true;
		else {
			global $db;
			getrow($db,"SELECT * FROM row_value WHERE value_value='".$folder.$prefix.$i.$ext."'",1,"row_value");
			if(!empty($db->Record)){
				if($genname==0) $i=$i+50+rand(1,20000)*10;
				if($genname==1) $i=uuin();
				if($genname==2){
					$i=clean_filename($filename).$z;
					$z++;
				}
			} else $ok=true;
		}
	}
	return $folder.$prefix.$i.$ext;
}

function check_dir($dir,$use_docroot=1){
	if(substr($dir,0,strlen(DOCUMENT_ROOT))==DOCUMENT_ROOT) $use_docroot=0;
	$d=explode('/',$dir);
	$cd='';
	$d2='';
	for($i=0;$i<count($d);$i++)if(!empty($d[$i])){
		if(empty($cd)){
			if(!$use_docroot && !strstr($dir,':')) $cd='/';
			$cd.=$d[$i];
		} else $cd.='/'.$d[$i];
		if($use_docroot){
			if(!file_exists(DOCUMENT_ROOT.'/'.$cd)){
				mkdir(DOCUMENT_ROOT.'/'.$cd);
				chmod(DOCUMENT_ROOT.'/'.$cd,DEF_DRMOD);
			}
		} else {
			$d2.='/'.$d[$i];
			if(strlen(DOCUMENT_ROOT)<strlen($d2) || substr(DOCUMENT_ROOT,0,strlen($d2))!=$d2){
				if(!file_exists($cd)){
					//echo $cd.'!<br>';
					mkdir($cd);
					chmod($cd,DEF_DRMOD);
				}
			}
		}
	}
}

function clone_file($src,$folder,$prefix){
	if(empty($src)) return '';
	check_dir($folder);
	$nname=find_name($folder,$prefix,get_ext($src),1);
	$res=copy(DOCUMENT_ROOT.$src,DOCUMENT_ROOT.$nname);
	chmod(DOCUMENT_ROOT.$nname,DEF_CHMOD);
	if($res) return $nname; else return '';
}

function load_file($path){
	$f=fopen($path,'r');
	$str=fread($f, filesize($path));
	fclose($f);
	return $str;
}

function bom_remove(){
    function duff($filename) {
        $symbols = chr(239) . chr(187) . chr(191);
        $file = file($filename);
        if (isset($file[0]) && strlen($file[0]) > 0 && strpos($file[0], $symbols) === 0) {
            $file[0] = str_replace($symbols, '', $file[0]);
            $handle = fopen($filename, 'w');
            fwrite($handle, join('', $file));
            fclose($handle);
        }
    }
    function duif($dir) {
        $ext = array('.php', '.inc');
        $files = scan_dir($dir);
        foreach ($files as $file) {
            if (!in_array($file, array('.', '..'))) {
                if (is_dir($file)) {
                    duif($file);
                } else {
                    if (in_array(substr($file, strrpos($file, '.')), $ext)) {
                        duff($file);
                    }
                }
            }
        }
    }
    duif(DOCUMENT_ROOT.'/core');
    duff(DOCUMENT_ROOT.'/index.php');
}

function ungzip_folders($data,$root){
	$sep1='^^^'.'!^^^';
	$sep2='^^^'.'?^^^';
	$sep3='^^^'.'*^^^';
	$sep4='^^^'.'$^^^';
	$sep5='^^^'.'#^^^';
	$sep6='^^^'.'@^^^';
	$r=$data;
	$fldrs=explode($sep1,gzinflate(base64_decode($r)));
	foreach($fldrs AS $fldr){
		$fldr=explode($sep2,$fldr);
		$folder=$fldr[0];
		if(!empty($fldr[1]))$files=explode($sep3,$fldr[1]);
		else $files=Array();
		check_dir($root.'/'.$folder);
		//echo $folder.'<br>';
		if(!empty($files) && !empty($fldr[1])) foreach($files AS $fil){
			$fil=explode($sep4,$fil);
			$f=fopen($root."/".$folder."/".$fil[0],"w");
			fwrite($f,$fil[1]);fclose($f);
			//echo $root.' - '.$folder.' - '.$fil[0].'<br>';
			chmod($root."/".$folder."/".$fil[0],DEF_CHMOD);
		}
	}
}

function TarAddHeader($f,$phisfn,$archfn) {
     $info=stat($phisfn);     $ouid=sprintf("%6s ", decoct($info[4]));     $ogid=sprintf("%6s ", decoct($info[5]));     $omode=sprintf("%6s ", decoct(fileperms($phisfn)));     $omtime=sprintf("%11s", decoct(filemtime($phisfn)));     if (@is_dir($phisfn))     {          $type="5";          $osize=sprintf("%11s ", decoct(0));     }     else     {          $type='';          $osize=sprintf("%11s ", decoct(filesize($phisfn)));          clearstatcache();     }     $dmajor = '';     $dminor = '';     $gname = '';     $linkname = '';     $magic = '';     $prefix = '';     $uname = '';     $version = '';     $chunkbeforeCS=pack("a100a8a8a8a12A12",$archfn, $omode, $ouid, $ogid, $osize, $omtime);     $chunkafterCS=pack("a1a100a6a2a32a32a8a8a155a12", $type, $linkname, $magic, $version, $uname, $gname, $dmajor, $dminor ,$prefix,'');     $checksum = 0;     for ($i=0; $i<148; $i++) $checksum+=ord(substr($chunkbeforeCS,$i,1));     for ($i=148; $i<156; $i++) $checksum+=ord(' ');     for ($i=156, $j=0; $i<512; $i++, $j++)    $checksum+=ord(substr($chunkafterCS,$j,1));     fwrite($f,$chunkbeforeCS,148);     $checksum=sprintf("%6s ",decoct($checksum));     $bdchecksum=pack("a8", $checksum);     fwrite($f,$bdchecksum,8);     fwrite($f,$chunkafterCS,356);     return true;
}

function TarWriteContents($f,$phisfn) {
     if (@is_dir($phisfn))     {         return;     }     else     {         $size=filesize($phisfn);         $padding=$size % 512 ? 512-$size%512 : 0;         $f2=fopen($phisfn,"rb");         while (!feof($f2)) fwrite($f,fread($f2,1024*1024));         $pstr=sprintf("a%d",$padding);         fwrite($f,pack($pstr,''));     }
}

function TarAddFooter($f) {
     fwrite($f,pack('a1024',''));
}

function gzip_folders2($dest,$fldrs,$sub=0,$inst=0,$except=Array()){//dest = адрес конечного архива, fldrs - массив папок, sub - включать подпапки, inst - включать htaccess
	$fols=Array();
	if(file_exists($dest)) unlink($dest);
	foreach($fldrs AS $fld){
		$fls=scan_dir(DOCUMENT_ROOT.'/'.$fld,Array(),$sub,2,$inst,1);
		foreach($fls AS $fl){
			$old_t=dirname($fl).'/'.end(explode('/',$fl));
			//if($old_t[0]=='/') $old_t=substr($old_t,1);
			$t=str_replace(DOCUMENT_ROOT.'/','',$old_t);
			//if($t==$old_t) $t=str_replace(dirname(dirname(dirname(__FILE__))),'',$t);
			if($t[0]=='/') $t=substr($t,1);
			$c=false;
			if(!empty($except)) foreach($except AS $e) if(substr($t,0,strlen($e))==$e){
				$c=true;
				break;
			}
			if($c) continue;
			if(empty($except[$t])) $fols[$t]=1;
		}
	}
	$f=gzopen($dest,'wb9');
	foreach($fols AS $fl=>$var){
		//if($fl[0]=='/') $fl=substr($fl,1);
		TarAddHeader($f,DOCUMENT_ROOT.'/'.$fl,$fl);
		TarWriteContents($f,DOCUMENT_ROOT.'/'.$fl);
	}
	TarAddFooter($f);
	gzclose($f);
}

function gzip_folders($fldrs,$except=Array(),$false_path=Array(),$self_extract=1,$wait=0,$hello='',$sub=0,$inst=0,$noup=1,$access=0,$double_check=0,$replace_dir='',$disallow_paths=Array()){//fldrs - массив папок (подпапки определяются вручную), except - название файлов, которые нельзя включать, false_path - названия папок, которые будут передаваться пустыми (указывается отдельно от fldrs),  self_extract - будет ли архив самораспаковывающимся, wait - ждать ли завершения установки, hello - сообщение, которое выводится по завершении работы (заключённое в html), sub - включать ли подпапки, inst - только для установки (включает .htaccess), noup- использовать резервные файлы, вместо тех, которые запрещены для обновления
	$sep1='^^^'.'!^^^';
	$sep2='^^^'.'?^^^';
	$sep3='^^^'.'*^^^';
	$sep4='^^^'.'$^^^';
	$sep5='^^^'.'#^^^';
	$sep6='^^^'.'@^^^';
	$r='';
	if(empty($disallow_paths)) $disallow_paths=$GLOBALS['DISALLOW_PATHS'];
	global $no_update;
	$first1=true;
	$e=Array();
	foreach($except AS $e2) $e[$e2]=1;
	$fols=Array();
	foreach($fldrs AS $index=>$fld)/*if(!empty($fld))*/{
		if(!empty($fld[0]) && $fld[0]=='!'){
			$fls=scan_dir(DOCUMENT_ROOT.'/'.substr($fld,1),Array(),1,2,$inst);
			$fldrs[$index]=substr($fld,1);
		} else{
			if(!is_dir(DOCUMENT_ROOT.'/'.$fld)){
				$fls=Array(DOCUMENT_ROOT.'/'.$fld); //поддержка обычных файлов, не папок
			}
			else {
				$fls=scan_dir(DOCUMENT_ROOT.'/'.$fld,Array(),$sub,2,$inst);
			}
		}
		//вторая проверка (сомнительная)
		if($double_check){
			$tmp=$fls;
			$fls=Array();
			foreach($tmp AS $f){
				$of=$f;
				$tp=substr($f,strlen(DOCUMENT_ROOT)+1);
				if(!empty($GLOBALS["DISALLOW_FILES"][$tp])) continue;
				if(!is_dir($f)){
					$f=dirname($f);
					$tp=substr($f,strlen(DOCUMENT_ROOT)+1);
				}
				if(!empty($disallow_paths[$tp])) continue;
				$tpa=explode('/',$tp);
				$fbreak=false;
				for($i=count($tpa);$i>0;$i--){
					$tpp=implode('/',$tpa);
					if(!empty($disallow_paths[$tpp]) && $tpp!=''){$fbreak=true; break;}
					array_pop($tpa);
				}
				if($fbreak) continue;
				$fls[]=$of;
			}
			foreach($GLOBALS['UPDATE_FILES'] AS $path){
				if(file_exists(DOCUMENT_ROOT.'/'.$path) && is_file(DOCUMENT_ROOT.'/'.$path)){
					$fls[]=DOCUMENT_ROOT.'/'.$path;
				}
			}
		}
		foreach($fls AS $f){
			//if(!empty($replace_dir)) $fols[str_replace(substr(DOCUMENT_ROOT.$replace_dir,0,strlen(DOCUMENT_ROOT.$replace_dir)-1),'',dirname($f))][]=end(explode('/',$f));
			/*else */
			if(!is_dir($f)) $fols[str_replace(DOCUMENT_ROOT,'',dirname($f))][]=end(explode('/',$f));
		}
	}
	foreach($fols AS $fld=>$fls){
		if(!$first1) $r.=$sep1;
		$fld2=$fld;
		if(!empty($replace_dir)){
			$fld2=str_replace(substr($replace_dir,0,strlen($replace_dir)-1),'',$fld2);
		}
		while(strstr($fld2,'//')) $fld2=str_replace('//','/',$fld2);
		//echo 'Folder: '.$fld2.'<br>';
		$r.=$fld2.$sep2;
		$first1=false;
		//$fls=scan_dir(DOCUMENT_ROOT.'/'.$fld,Array(),$sub,2,$inst);
		$first2=true;
		foreach($fls AS $f){
			//if($fld!='') $f2=str_replace(DOCUMENT_ROOT.'/'.$fld.'/','',$f); else $f2=str_replace(DOCUMENT_ROOT.'/','',$f);
			$f2=$f;
			if($fld!='') $f=DOCUMENT_ROOT.'/'.$fld.'/'.$f; else $f=DOCUMENT_ROOT.'/'.$f;
			if(empty($e[$f2])){
				if(!$first2) $r.=$sep3;
				$r.=$f2.$sep4;
				//echo '- '.$f2.'<br>';
				$first2=false;
				if($noup){
					if($fld=='') $f3=$f2; else $f3=substr($fld,1).'/'.$f2;
					if(isset($no_update[$f3]) && file_exists(DOCUMENT_ROOT.'/core/units/safe/'.md5($f3))){
						$f=DOCUMENT_ROOT.'/core/units/safe/'.md5($f3);
					}
				}
				$fh=fopen($f,'rb');
				//if(filesize($f)==0) echo $f.'<br>';
				$r.=fread($fh, filesize($f));
				fclose($fh);
			}
		}
	}
	//exit;
	foreach($false_path AS $fld){
		if(!$first1) $r.=$sep1;
		$r.=$fld.$sep2;
		$first1=false;
	}
	$r=base64_encode(gzdeflate($r));
	if($self_extract){
		$acc='define("DEF_CHMOD",0666);define("DEF_DRMOD",0777);';
		if($access) $acc='define("DEF_CHMOD",0666);define("DEF_DRMOD",0777);';
		$r='<?php
				if(strstr(__FILE__,"!")){
					header("Content-Type: application/force-download");
					header("Content-Type: application/octet-stream");
					header("Content-Type: application/download");
					header("Content-Disposition: attachment;filename=".str_replace("!","",basename(__FILE__))); 
					header("Content-Transfer-Encoding: binary");
					header("Connection: close");
					ob_start();
					echo file_get_contents(__FILE__);
					$size=ob_get_length();
					header("Content-Length: $size");
					if($size>0) ob_end_flush();
					flush();
					ob_start();
					ob_end_clean();
					exit;					
				}
				header("Content-type: text/html; charset=utf-8");
				define("DOCUMENT_ROOT", dirname(__FILE__));
				'.$acc.'

				$wait='.$wait.';
				if(!$wait){
					header("Connection: close");
					ob_start();
					echo \'<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html xmlns="http://www.w3.org/1999/xhtml"><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head><body>'.str_replace("'","\\'",$hello).'</body></html>\';
					$size=ob_get_length();
					header("Content-Length: $size");
					if($size>0) ob_end_flush();
					flush();
					ob_start();
					ob_end_clean();
				}

				function check_dir($dir){
					$d=explode("/",$dir);
					$cd="";
					$x=DOCUMENT_ROOT; if($x[strlen($x)-1]=="/") $x=""; else $x="/";
					for($i=0;$i<count($d);$i++)if(!empty($d[$i])){
						if(empty($cd)) $cd=$d[$i]; else $cd.="/".$d[$i];
						if(!file_exists(DOCUMENT_ROOT.$x.$cd)){
							mkdir(DOCUMENT_ROOT.$x.$cd);
							chmod(DOCUMENT_ROOT.$x.$cd,DEF_DRMOD);
						}
					}
				}
				
				$ba=@fopen(dirname(__FILE__)."/e5_tmp","w");
				if(!$ba){
					echo "<h2>Недостаточно прав для завершения операции</h2><p>Установите права для корневой папки в 755 от имени PHP-пользователя, либо в 777 от имени FTP-пользователя</p><p>Также заранее проверьте доступ на файл index.php. Установите его в 644 от имени PHP-пользователя, либо в 777 от имени FTP-пользователя.</p>";
					exit;
				} else {
					fclose($ba);
					unlink(dirname(__FILE__)."/e5_tmp");
				}
				$b=fopen(__FILE__,"rb");
				$x=DOCUMENT_ROOT; if($x[strlen($x)-1]=="/") $x=""; else $x="/";
				$r=explode("^^^#"."^^^",fread($b,filesize(__FILE__)));
				fclose($b);
				$ba=@unlink(__FILE__);
				if(!$ba){
					echo "<h2>Недостаточно прав для завершения операции</h2><p>Установите права для файла index.php в 644 от имени PHP-пользователя, либо в 777 от имени FTP-пользователя</p>";
					exit;
				}
				$fldrs=explode("'.$sep1.'",gzinflate(base64_decode($r[1])));
				@chmod(DOCUMENT_ROOT,DEF_DRMOD);
				$start=DOCUMENT_ROOT.$x;
				@$f=fopen($start."test1","w");
				@fclose($f);
				if(file_exists($start."test1")) @unlink($start."test1");
				else {$start=""; @unlink("test1");}
				foreach($fldrs AS $fldr){
					$fldr=explode("'.$sep2.'",$fldr);
					$folder=$fldr[0];
					$files=explode("'.$sep3.'",$fldr[1]);
					check_dir($folder);
					if(!empty($files) && !empty($fldr[1])) foreach($files AS $fil){
						$fil=explode("'.$sep4.'",$fil);
						if($folder!=""){
							$xn=$start.$folder;
							$xd="/";
							if(!empty($xn[strlen($xn)-1]) && $xn[strlen($xn)-1]=="/") $xd="";
							$f=fopen($start.$folder.$xd.$fil[0],"w");
							fwrite($f,$fil[1]);fclose($f);
							chmod($start.$folder.$xd.$fil[0],DEF_CHMOD);
						} else {
							$f=fopen($start.$fil[0],"w");
							fwrite($f,$fil[1]);fclose($f);
							chmod($start.$fil[0],DEF_CHMOD);
						}
					}
				}

				if($wait){
					header("Connection: close");
					ob_start();
					echo \'<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html xmlns="http://www.w3.org/1999/xhtml"><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head><body>'.str_replace("'","\\'",$hello).'</body></html>\';
					$size=ob_get_length();
					header("Content-Length: $size");
					if($size>0) ob_end_flush();
					flush();
					ob_start();
					ob_end_clean();
				}
				exit;
			?>
		'.$sep5.$r;
	}
	return $r;
}

function f_connect($fs){
	global $allow_ftp;
	if(!isset($GLOBALS["allow_ftp"])) $allow_ftp=1;
	if($allow_ftp==1){	//BASIC PHP FTP FUNCTIONS
			@$f=ftp_connect($fs);
			return $f;
	}
	if($allow_ftp==0){	//SOCKET VERSION
		require_once(DOCUMENT_ROOT."/core/units/left/ftp.php");
		$f=new FTP($fs);
		return $f;
	}
	if($allow_ftp==2){	//CURL VERSION
		$ftp->fs=$fs;
		return $ftp;
	}
}

function f_login($f,$fl,$fp){
	global $allow_ftp;
	if($allow_ftp==1){
		@$r=ftp_login($f,$fl,$fp);
		return $r;
	}
	if($allow_ftp==0){
		$f->user=$fl;
		$f->pass=$fp;
		$sock = $f->connect();		
		if($sock)	if($f->is_ok()==1)  if($f->login($f->user,$f->pass)) return 1;
		return 0;
	}
	if($allow_ftp==2){
		$f->fl=$fl;
		$f->fp=$fp;
		return 1;
	}
}

function f_put($f,$to,$from,$mode=0){
	global $allow_ftp;
	if($allow_ftp==1){
		return @ftp_put($f,$to,$from,$mode);
	}
	if($allow_ftp==0){
		return $f->stor($from,$to);
	}
	if($allow_ftp==2){
		$ftpLogin=$f->fl;
		$ftpPass=$f->fp;
		$ftpAddr=$f->fs;
		$ftpFile=$from;
		$remoteurl = "ftp://$ftpLogin:$ftpPass@$ftpAddr:21/$to";
		$ch = curl_init();
		$fp = fopen($ftpFile, "rb");
		curl_setopt($ch, CURLOPT_URL, $remoteurl);
		curl_setopt($ch, CURLOPT_UPLOAD, 1);
		curl_setopt($ch, CURLOPT_INFILE, $fp);
		curl_setopt($ch, CURLOPT_INFILESIZE, filesize($ftpFile));
		$error = curl_exec($ch);
		curl_close($ch);
		return $error;
	}
}

function f_delete($f,$file){
	global $allow_ftp;
	if($allow_ftp==1){
		return ftp_delete($f,$file);
	}
	if($allow_ftp==0){
		return $f->dele($file);
	}
	if($allow_ftp==2){
		$ch = curl_init(); 
		curl_setopt($ch, CURLOPT_URL, "ftp://".$f->fs); 
		curl_setopt($ch, CURLOPT_USERPWD, $f->fl.":".$f->fp);
		curl_setopt($ch, CURLOPT_QUOTE, array('DELE /'.$file, 'QUIT')); 
		curl_exec($ch); 
		curl_close($ch);
	}
}

function f_nlist($f,$dir){
	global $allow_ftp;
	if($allow_ftp==1){
		return @ftp_nlist($f,$dir);
	}
	if($allow_ftp==0){
		return show_list($f->dir_list($dir));
	}
	if($allow_ftp==2){
		$ch = curl_init();
		require_once(DOCUMENT_ROOT."/core/units/left/ftp.php");
		if($dir!='') $dir='/'.$dir.'/'; else $dir='';
		curl_setopt($ch, CURLOPT_URL, "ftp://".$f->fs.$dir);
		curl_setopt($ch, CURLOPT_USERPWD, $f->fl.":".$f->fp);
		//curl_setopt($ch, CURLOPT_FTPLISTONLY, TRUE); 
	 	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
		$res=curl_exec($ch);
		$list = explode("\n",$res);
		$pattern = "/[dwrx\-]{10}/";
		$res=Array();
		$list = array_slice($list,1,count($list)-1);
		foreach($list as $file){
			if($file == '')continue;
			$file = preg_split("/ /",$file,20,PREG_SPLIT_NO_EMPTY);
			if(preg_match('/^d/',trim($file[0]))){
				$res[]=str_replace("\r","",str_replace("\n","",$file[8]));
			}
		}
		curl_close($ch);
		return $res;
	}
}

function f_close($f){
	global $allow_ftp;
	if($allow_ftp==1){
		ftp_close($f);
	} else {
		
	}
}

if (!function_exists('ftp_chmod')) {
     function ftp_chmod($ftp_stream, $mode, $filename)
     {
         return ftp_site($ftp_stream, sprintf('CHMOD %u %s', $mode, $filename));
     }
 }

function f_chmod($f,$chmod,$file){
	global $allow_ftp;
	if($allow_ftp==1){
		ftp_chmod($f,$chmod,$file);
		//ftp_site($ftp_stream, sprintf('CHMOD %u %s', $mode, $file));
	} else if($allow_ftp==0) {

	} else if($allow_ftp==2)	{
		$ch = curl_init(); 
		curl_setopt($ch, CURLOPT_URL, "ftp://".$f->fs); 
		curl_setopt($ch, CURLOPT_USERPWD, $f->fl.":".$f->fp);
		//curl_setopt($ch, CURLOPT_QUOTE, array('CHMOD '.$file.' '.decoct($chmod), 'QUIT')); 
		curl_setopt($ch, CURLOPT_QUOTE, array('SITE chmod '.decoct($chmod).' /'.$file, 'QUIT')); 
		curl_exec($ch); 
		curl_close($ch);
	}
}

function  folder_to_data($folder){
	if(empty($folder)) return $folder;
	return  $folder.'|_|_|'.gzip_folders(Array($folder),Array(),Array(),0,0,'',1,0,1,0,0,$folder);
}

function data_to_folder($data){
	if(!strpos($data,'|_|_|')) return $data;
	$tmp=explode('|_|_|',$data);
	$folder=$tmp[0];
	$data=$tmp[1];
	if($folder[0]=='/') $folder=substr($folder,1);
	if($folder[strlen($folder)-1]=='/') $folder=substr($folder,0,strlen($folder)-1);
	$folder=explode('/',$folder);
	$i=1;
	$last_folder=$folder[count($folder)-1];
	while(file_exists(DOCUMENT_ROOT.'/'.implode('/',$folder))){
		$folder[count($folder)-1]=$last_folder.'_'.$i;
		$i++;
	}
	$folder='/'.implode('/',$folder);
	check_dir(DOCUMENT_ROOT.$folder);
	ungzip_folders($data,DOCUMENT_ROOT.$folder);
	return $folder.'/';
}

$files_to_data_cache=Array();
function file_to_data($filename){
	global $files_to_data_cache;
	if(!empty($files_to_data_cache[$filename])) return '^'.$filename;
	else $files_to_data_cache[$filename]=1;
	if(empty($filename)) return '';
	$pref=DOCUMENT_ROOT;
	if(file_exists($pref.$filename)) return $filename.'|'.base64_encode(file_get_contents($pref.$filename));
	else return '';
}

$data_to_file_cache=Array();
function data_to_file($data){
	global $data_to_file_cache;
	$pref=DOCUMENT_ROOT;
	if(!empty($data)){
		if($data[0]!='^'){
			$data=explodeA('|',$data,'','',1);
			//unique name
			$tmp_filename=$data[0];
			$filename=explode('.',$data[0]);
			if(count($filename)>1){
				$ext='.'.array_pop($filename);
				$filename=implode('.',$filename);
			} else {
				$filename=$filename[0];
				$ext='';
			}
			if(file_exists($pref.$filename.$ext)){
				$i=1;
				while(file_exists($pref.$filename.$i.$ext)) $i++;
				$filename=$filename.$i;
			}
			$filename.=$ext;
			//check folder
			check_dir($pref.dirname($filename),0);
			//save data
			$data=base64_decode($data[1]);
			$f=fopen($pref.$filename,'w+');
			fwrite($f,$data);
			fclose($f);
			$data_to_file_cache[$tmp_filename]=$filename;
			return $filename;
		} else {
			if(!empty($data_to_file_cache[substr($data,1)])) return $data_to_file_cache[substr($data,1)];
			else return '';
		}
	} else return '';
}

function echo_file($data,$name){
	ob_end_clean();
	ob_start();
	header ("Content-type: octet/stream"); 
	header ("Content-disposition: attachment; filename=".$name); 
	header("Content-Length: ".strlen($data));
	echo $data;
	exit;
}

function file_get_contents2($url){//safe
	$all_good=false;
	$turl=strtolower($url);
	if(strstr($turl,'core')){
		$accepted='qwertyuiopasdfghjklzxcvbnm-_()';
		$tmpz=explode('core',$turl);
		if(!empty($tmpz[0]) && strstr($accepted,$tmpz[0][strlen($tmpz[0])-1])) $all_good=true;
		if(!empty($tmpz[1]) && strstr($accepted,$tmpz[1][0])) $all_good=true;
	} else $all_good=true;
	if($all_good) return file_get_contents($url);
	else return '';
}

function get_free_mem(){
	if(@file_exists("/proc/meminfo")){
		$d=explode("\n",@file_get_contents("/proc/meminfo"));
		if(empty($d)) return '';
		foreach($d AS $t){
			$t=explode(':',$t);
			//echo '!'.$t[0].'-'.$t[1].'!';
			if($t[0]=='MemFree'){
				$t[1]=str_replace('	',' ',$t[1]);
				while(strstr($t[1],'  ')) $t[1]=str_replace('  ',' ',$t[1]);
				$t[1]=trim($t[1]);
				$t=explode(' ',$t[1]);
				if(empty($t[0])) return '';
				return $t[0].'000';
			}
		}
	} else return '';
}

function testfile($text,$num='',$cmd='w+'){
	$f=fopen(DOCUMENT_ROOT.'/testfile'.$num.'.txt',$cmd);
	fwrite($f,$text);
	fclose($f);
}

function gzdecode2($data,$filename='',$error='',$maxlength=null){
     $len = strlen($data);
     if ($len < 18 || strcmp(substr($data,0,2),"\x1f\x8b")) {
         $error = "Not in GZIP format.";
         return null;  // Not GZIP format (See RFC 1952)
     }
     $method = ord(substr($data,2,1));  // Compression method
     $flags  = ord(substr($data,3,1));  // Flags
     if ($flags & 31 != $flags) {
         $error = "Reserved bits not allowed.";
         return null;
     }
     // NOTE: $mtime may be negative (PHP integer limitations)
     $mtime = unpack("V", substr($data,4,4));
     $mtime = $mtime[1];
     $xfl   = substr($data,8,1);
     $os    = substr($data,8,1);
     $headerlen = 10;
     $extralen  = 0;
     $extra     = "";
     if ($flags & 4) {
         // 2-byte length prefixed EXTRA data in header
         if ($len - $headerlen - 2 < 8) {
             return false;  // invalid
         }
         $extralen = unpack("v",substr($data,8,2));
         $extralen = $extralen[1];
         if ($len - $headerlen - 2 - $extralen < 8) {
             return false;  // invalid
         }
         $extra = substr($data,10,$extralen);
         $headerlen += 2 + $extralen;
     }
     $filenamelen = 0;
     $filename = "";
     if ($flags & 8) {
         // C-style string
         if ($len - $headerlen - 1 < 8) {
             return false; // invalid
         }
         $filenamelen = strpos(substr($data,$headerlen),chr(0));
         if ($filenamelen === false || $len - $headerlen - $filenamelen - 1 < 8) {
             return false; // invalid
         }
         $filename = substr($data,$headerlen,$filenamelen);
         $headerlen += $filenamelen + 1;
     }
     $commentlen = 0;
     $comment = "";
     if ($flags & 16) {
         // C-style string COMMENT data in header
         if ($len - $headerlen - 1 < 8) {
             return false;    // invalid
         }
         $commentlen = strpos(substr($data,$headerlen),chr(0));
         if ($commentlen === false || $len - $headerlen - $commentlen - 1 < 8) {
             return false;    // Invalid header format
         }
         $comment = substr($data,$headerlen,$commentlen);
         $headerlen += $commentlen + 1;
     }
     $headercrc = "";
     if ($flags & 2) {
         // 2-bytes (lowest order) of CRC32 on header present
         if ($len - $headerlen - 2 < 8) {
             return false;    // invalid
         }
         $calccrc = crc32(substr($data,0,$headerlen)) & 0xffff;
         $headercrc = unpack("v", substr($data,$headerlen,2));
         $headercrc = $headercrc[1];
         if ($headercrc != $calccrc) {
             $error = "Header checksum failed.";
             return false;    // Bad header CRC
         }
         $headerlen += 2;
     }
     // GZIP FOOTER
     $datacrc = unpack("V",substr($data,-8,4));
     $datacrc = sprintf('%u',$datacrc[1] & 0xFFFFFFFF);
     $isize = unpack("V",substr($data,-4));
     $isize = $isize[1];
     // decompression:
     $bodylen = $len-$headerlen-8;
     if ($bodylen < 1) {
         // IMPLEMENTATION BUG!
         return null;
     }
     $body = substr($data,$headerlen,$bodylen);
     $data = "";
     if ($bodylen > 0) {
         switch ($method) {
         case 8:
             // Currently the only supported compression method:
             @$data = gzinflate($body,$maxlength);
             break;
         default:
             $error = "Unknown compression method.";
             return false;
         }
     }  // zero-byte body content is allowed
     // Verifiy CRC32
     $crc   = sprintf("%u",crc32($data));
     $crcOK = $crc == $datacrc;
     $lenOK = $isize == strlen($data);
     if (!$lenOK || !$crcOK) {
         $error = ( $lenOK ? '' : 'Length check FAILED. ') . ( $crcOK ? '' : 'Checksum FAILED.');
         return false;
     }
     return $data;
 }

function gzdecode1($data){
	if(empty($data)) return false;
	if(substr($data,0,4)!='!ngz'){
		if(!function_exists('gzdecode')){
			$res=gzdecode2($data);
			return $res;
		}
		else return gzdecode($data);
	}
	else return substr($data,4,strlen($data)-4);
}

function loadserv($url){
	if(!function_exists('gzdecode')){
		if(strstr($url,'?')) $url.='&nogzip=1';
		else $url.='?nogzip=1';
	}
	@$res=file_get_contents($url);
	if(empty($res) && function_exists('curl_init')) @$res=curl_get($url,0);
	if(empty($res) && function_exists('fsockopen')){
		@$res=do_get($url);
	}
	//if(empty($res)){
	//	@$res=curl_get($url);
	//}
	$res=gzdecode1($res);
	return $res;
}

function new_rand_name($row,$col){
	global $db;
	getrow($db,"SELECT * FROM main_col WHERE col_id=$col");
	$prefix=$db->Record["file_prefix"];
	getrow($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col=$col");
	$fn=$db->Record["value_value"];
	$fn_id=$db->Record["value_id"];
	if(!empty($fn) && file_exists(DOCUMENT_ROOT.$fn) && !is_dir(DOCUMENT_ROOT.$fn)){
		$dn=dirname($fn);
		$x=find_name($dn.'/',$prefix,get_ext($fn),1,1,'');
		rename(DOCUMENT_ROOT.$fn,DOCUMENT_ROOT.$x);
		$db->query("UPDATE row_value SET value_value='$x' WHERE value_value='$fn'");//опасно
		clear_rowval_cache($col,$row);
		return $x;
	}
	return false;
}

function clean_sql(){
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(is_dir($obj)){
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name!='tpl' && $name!='tpl_struct' && $name!='url') removeDirRec($obj);
	}
        }
    }
}

function check_for_upload_file_col($col,$prefix='',$filename=''){
	foreach($col AS $var=>$value) $$var=$value;
	if(isset($_FILES[$prefix."col".$col_id]["tmp_name"])){
		$tmp_name=$_FILES[$prefix."col".$col_id]["tmp_name"];
		$name=$_FILES[$prefix."col".$col_id]["name"];
	} else if(isset($_POST[$prefix."col".$col_id]["tmp_name"]) && is_array($_POST[$prefix."col".$col_id])){ //это чтобы можно было добавлять файлы через addrow
		$tmp_name=$_POST[$prefix."col".$col_id]["tmp_name"];
		$name=$_POST[$prefix."col".$col_id]["name"];
	} else if(isset($_FILES[$prefix.$col_sname])){
		$tmp_name=$_FILES[$prefix.$col_sname]["tmp_name"];
		$name=$_FILES[$prefix.$col_sname]["name"];
	}
	if(empty($tmp_name) && !empty($filename) && !empty($_FILES)){
		foreach($_FILES AS $v=>$f){
			if($f["name"]==$filename || $f["tmp_name"]==$filename || $v==$filename){
				$tmp_name=$f["tmp_name"];
				$name=$f["name"];
				break;
			}
		}
	}
	return !empty($tmp_name);
}

function upload_file_col($col,$prefix='',$filename=''){
	foreach($col AS $var=>$value) $$var=$value;
	// файл
	$tmp_name='';
	//if(!empty($_FILES[$prefix."col".$col_id]["tmp_name"]) || !empty($_FILES[$prefix.$col_sname]["tmp_name"]) || !empty($_POST[$prefix."col".$col_id]["tmp_name"])){
	//} else return false;
	//foreach($_FILES AS $var=>$value) echo $var.' = '.implode(', ',$value).'<br>';
	if(isset($_FILES[$prefix."col".$col_id]["tmp_name"])){
		$tmp_name=$_FILES[$prefix."col".$col_id]["tmp_name"];
		$name=$_FILES[$prefix."col".$col_id]["name"];
	} else if(isset($_POST[$prefix."col".$col_id]["tmp_name"]) && is_array($_POST[$prefix."col".$col_id])){ //это чтобы можно было добавлять файлы через addrow
		$tmp_name=$_POST[$prefix."col".$col_id]["tmp_name"];
		$name=$_POST[$prefix."col".$col_id]["name"];
	} else if(isset($_FILES[$prefix.$col_sname])){
		$tmp_name=$_FILES[$prefix.$col_sname]["tmp_name"];
		$name=$_FILES[$prefix.$col_sname]["name"];
	}
	
	if($tmp_name=='1' || strlen($tmp_name)==1) $tmp_name='';	//неизвестный косяк - скорее всего вызван обработкой строковой POST переменной как массива
	
	//проверяем наличие файла по имени
	if(empty($tmp_name) && !empty($filename) && !empty($_FILES)){
		foreach($_FILES AS $v=>$f){
			if($f["name"]==$filename || $f["tmp_name"]==$filename || $v==$filename){
				$tmp_name=$f["tmp_name"];
				$name=$f["name"];
				break;
			}
		}
	}
	
	//закачиваем файл из другой папки
	if(empty($tmp_name) && !empty($filename)){
		if(check_filename_safe($filename)){
			$tmp_name=fix_filename($filename);
			$name=basename($tmp_name);
		}
	}
	
	if(empty($tmp_name)) return false;
	
	if(is_array($tmp_name)) $tmp_name=$tmp_name[0];
	if(is_array($name)) $name=$name[0];
	if(file_exists($tmp_name)){
		if(!empty($file_types)){
			$file_types=str_replace(' ','',$file_types);
			if(strstr($file_types,',')){
				$file_types=array_flip(explode(',',strtolower($file_types)));
			} else $file_types=Array(strtolower($file_types)=>1);
			$e=get_ext2($name);
			if(!empty($e) && !isset($file_types[strtolower($e)])){
				$GLOBALS["cancel"]='Запрещено использовать данное расширение для файла «'.$col_name.'»';
				return -1;
			}
		}
		if(filesize($tmp_name)>($file_maxsize*1024) && $file_maxsize!=0){
			$GLOBALS["cancel"]='Превышен максимальный размер для файла «'.$col_name.'»';
			return -1;
		}
		if((sizeDirRec(DOCUMENT_ROOT.$file_dir)+filesize($tmp_name))>($file_totalmax*1024) && $file_totalmax!=0){
			$GLOBALS["cancel"]='Невозможно загрузить файл «'.$col_name.'» в папку '.$file_dir.', т.к. будет превышен максимальный размер папки';
			return -1;
		}		
		$nname=find_name($file_dir,$file_prefix,get_ext($name),1,$file_genname,$name);
		check_dir($file_dir);
		$r=move_uploaded_file($tmp_name,DOCUMENT_ROOT.$nname);
		if($r){
			chmod(DOCUMENT_ROOT.$nname,DEF_CHMOD);
			return $nname;
		} else return false;
	}
	return false;
}

function upload_file($filevar,$folder='/files/tmp/',$prefix='',$method=0){
	// файл
	$tmp_name='';
	if(isset($_FILES[$filevar]["tmp_name"])){
		$tmp_name=$_FILES[$filevar]["tmp_name"];
		$name=$_FILES[$filevar]["name"];
	} else if(isset($_POST[$filevar]["tmp_name"]) && is_array($_POST[$filevar])){
		$tmp_name=$_POST[$filevar]["tmp_name"];
		$name=$_POST[$filevar]["name"];
	} else if(isset($_FILES[$filevar])){
		$tmp_name=$_FILES[$filevar]["tmp_name"];
		$name=$_FILES[$filevar]["name"];
	}
	
	//проверяем наличие файла по имени
	if(empty($tmp_name) && !empty($filevar) && !empty($_FILES)){
		foreach($_FILES AS $v=>$f){
			if($f["name"]==$filevar || $f["tmp_name"]==$filevar || $v==$filevar){
				$tmp_name=$f["tmp_name"];
				$name=$f["name"];
				break;
			}
		}
	}
	
	//закачиваем файл из другой папки
	if(empty($tmp_name) && !empty($filevar)){
		if(check_filename_safe($filevar)){
			$tmp_name=fix_filename($filevar);
			$name=basename($tmp_name);
		}
	}
	
	if(empty($tmp_name)) return false;
	
	if(is_array($tmp_name)) $tmp_name=$tmp_name[0];
	if(is_array($name)) $name=$name[0];
	if(file_exists($tmp_name)){
		$nname=find_name($folder, $prefix, get_ext($name),0,$method,$name);
		check_dir($folder);
		$r=move_uploaded_file($tmp_name,DOCUMENT_ROOT.$nname);
		if($r){
			chmod(DOCUMENT_ROOT.$nname,DEF_CHMOD);
			return $nname;
		} else return false;
	}
	return false;
}

function fix_filename($filename){
	if(empty($filename)) return '';
	if($filename[0]=='/') return DOCUMENT_ROOT.$filename;
	else return DOCUMENT_ROOT.'/'.$filename;
}
	
function check_filename_safe($filename){
	if(empty($filename)) return false;
	if(strstr($filename,'core/')) return false;
	if(strstr($filename,'config.inc')) return false;
	if(strstr($filename,'deploy/')) return false;
	$fn=fix_filename($filename);
	if(!file_exists($fn)) return false;
	return true;
}

function set_cookie_file(&$curl,$cookie){
	global $user;
	if($cookie<0){
		$cookie=-$cookie;
		curl_setopt($curl,CURLOPT_COOKIESESSION,true);		
	}
	check_dir('core/cache/cookies');
	$cn='cookie';
	if(empty($user->id)) $uid='0';
	else $uid=$user->id;
	if(empty($user->ip)) $uip='192.168.1.1';
	else $uip=$user->ip;
	if($cookie==1) $cn.=$uid;
	else if($cookie==2) $cn.=$uip;
	else $cn.='num'.$cookie;
	$cn.='.txt';
	$cn=DOCUMENT_ROOT.'/core/cache/cookies/'.$cn;
	curl_setopt($curl, CURLOPT_COOKIEJAR,$cn);
}

function normalize_path($path){
	if(empty($path)) return '';
	if(strpos(strtolower($path),'core/')) $path='';
	if($path[0]=='/') $path=substr($path,1);
	if($path[strlen($path)-1]=='/') $path=substr($path,0,strlen($path)-1);
	return $path;
}

function list_files($folders,$files=Array()){
	$res=Array();
	foreach($folders AS $path){
		if($path!='' && $path[0]=='!'){
			$subfolders=true;
			$path=substr($path,1);
			if($path!='') $path.='/';
		} else $subfolders=false;
		$res=find_file($path,$res,$subfolders);
	}
	foreach($files AS $path){
		if(file_exists(DOCUMENT_ROOT.'/'.$path) && is_file(DOCUMENT_ROOT.'/'.$path)){
			$res[$path]->url=$path;
			$res[$path]->name=basename($path);
			$res[$path]->size=filesize(DOCUMENT_ROOT.'/'.$path);
		}
	}
	return $res;
}

function del_file($filename){
	if(!file_exists(DOCUMENT_ROOT.$filename)) return false;
	global $db;
	getrow($db,"SELECT count(*) AS cnt FROM row_value WHERE value_value='".$filename."'",1,"row_value");
	if($db->Record["cnt"]<=1){
		if($db->Record["cnt"]==1){
			//getrow($db,"SELECT * FROM row_value WHERE value_value='".$filename."'",1,"row_value");
			//getrow($db,"SELECT * FROM main_col WHERE col_id=".$db->Record["value_col"],1,"main_col");
			//if(substr($filename,0,strlen($db->Record["file_dir"]))!=$db->Record["file_dir"] && ($db->Record["file_prefix"]=='' || !strstr($filename,$db->Record["file_prefix"]))) return false;//защита сделана для того, чтобы не удалять файлы, выбранные руками из редактора файлов для полей, при изменении полей.
			getrow($db,"SELECT file_dir FROM main_col WHERE file_dir!='' AND file_dir='".dirname($filename).'/'."'",1,"main_col");
			if(empty($db->Record)) return false;
			//тут надо учитывать, что если префикс со временем сменился, то при обновлении этих файлов их прежние копии останутся не удалённым (эпик фейл)
			//а также если файл был передан из другого столбца (у которого соответственно другая папка, то файл также не удалится
			//вывод - нужно придумывать замену этой схеме
			
			//предложен другой вариант - искать в значении file_dir, если не найдено - выходим, найдено - удаляем
			
		}
		unlink(DOCUMENT_ROOT.$filename);
	} else return false;
	return true;
}

function set_col_file($data,$row,$c,$ext='',$prefix='',$is_ex=false){//col is array{	
	global $db,$rlink;
	$fname=rowcol_val($row,$c);
	//set existing file
	$data=prepend_file($data);

	if(!$is_ex){
		seek_rlink($row);
		$module_id=$rlink[$row]->module;
		$table_id=$rlink[$row]->table;
	} else {
		$module_id=getrowval("SELECT ex_module FROM ex_module WHERE ex_id=$row","ex_module");
		$table_id=0;
	}
	
	//checking for post files data	
	if(check_for_upload_file_col($c,$prefix,$data)){
		$value=upload_file_col($c,$prefix,$data);
		if($value==-1) return -1;
		if(!empty($fname)) del_file($fname);
		$value=prepend_file($value);
		$db->query('DELETE FROM row_value WHERE value_row='.$row.' AND value_col='.$c['col_id'].' AND value_table>0',3,'row_value',__LINE__,__FILE__);
		$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
			VALUES ('.$module_id.', '.$table_id.', '.$c['col_id'].','.$row.','."'".$value."'".')',3,'row_value',__LINE__,__FILE__);
		return $value;
	} elseif(!empty($data) && file_exists(DOCUMENT_ROOT.$data)){
		if(!empty($fname)) del_file($fname);
		$db->query('DELETE FROM row_value WHERE value_row='.$row.' AND value_col='.$c["col_id"].' AND value_table>0',3,'row_value',__LINE__,__FILE__);			
		$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
			VALUES ('.$module_id.', '.$table_id.', '.$c["col_id"].','.$row.',\''.$data.'\')',3,'row_value',__LINE__,__FILE__);
		return false;
	//creating new file with data content
	} else if(!empty($data)) {
		if(empty($fname) || file_exists(DOCUMENT_ROOT.$fname)){
			$fname=find_name($c["file_dir"],$c["file_prefix"],$ext,1,1);
			global $rlink;
			seek_rlink($row);
			$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
				VALUES ('.$module_id.', '.$table_id.', '.$c["col_id"].','.$row.',\''.$fname.'\')',3,'row_value',__LINE__,__FILE__);
		}
		$f=fopen(DOCUMENT_ROOT.$fname,'w');
		fwrite($f,$data);
		fclose($f);
		return $fname;
	}
}

function check_deploy_path(){
	check_dir(DEPLOY_PATH);
	$htaccess_url=DOCUMENT_ROOT.DEPLOY_PATH.'.htaccess';
	if(!file_exists($htaccess_url)){
		$f=fopen($htaccess_url,'w+');
		fwrite($f,DEPLOY_HTACCESS);
		fclose($f);
	}
}

//$ff=list_files($GLOBALS['UPDATE_PAHTS'],$GLOBALS['UPDATE_FILES']);
//foreach($ff AS $f) echo $f->url.'/'.$f->name.'<br>';

?>