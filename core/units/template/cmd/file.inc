<?php


$cmd['get'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(function_exists('curl_init')){
			if($curl = curl_init()){
				curl_setopt($curl, CURLOPT_URL, parse_var($op->put[0]));
				if(!empty($op->put[1])) set_cookie_file($curl,parse_var($op->put[1]));
				curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
				curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);
				curl_setopt($curl, CURLOPT_RETURNTRANSFER,true);
				$res = curl_exec($curl);
				curl_close($curl);
				return $res;
			} else return '';
		} else return file_get_contents(parse_var($op->put[0]));
};
$cmd['get'][CMD_FILE]->result=CMD_STRING;
$cmd['get'][CMD_FILE]->visual=Array('title'=>'Загрузить URL через GET','put'=>Array(
		0=>Array('title'=>'URL','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Правила использования cookies','type'=>CMD_STRING,'req'=>0)
));


$cmd['post'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if($curl = curl_init()){
	                $postdata=Array();
	                for($i=1;$i<count($op->put);$i++){
	                	$p=$op->put[$i];
                		$op1=$p->sub[0];
                		$op2=$p->sub[1];
				if($op1->data!='COOKIES' && $op1->data!='FILE') $name=parse_var($op1);
				if($op1->data=='COOKIES') set_cookie_file($curl,parse_var($op2));
				else if($op1->data=='FILE'){
					if($op2->type==LEXER_SUB && isset($op2->sub[0]->next_operation) && $op2->sub[0]->next_operation==':'){
						$name=parse_var($op2->sub[0]);
						$fn=realpath(parse_var($op2->sub[1]));
					} else {
						$fn=realpath(parse_var($op2));
						$name=basename($fn);
					}					
					if(!empty($fn) && !strpos(strtolower($fn),'core/')){
						if($fn[0]!='/') $fn=DOCUMENT_ROOT.'/'.$fn;
						else $fn=DOCUMENT_ROOT.$fn;
						if(file_exists($fn)) $postdata[$name]='@'.$fn;
					}
				} else {
					$postdata[parse_var($op1)]=parse_var($op2);//тут возможно не надо делать в левой части parse_var и вместо неё сделать op1->data
				}
	                }
			curl_setopt($curl, CURLOPT_URL, parse_var($op->put[0]));
			curl_setopt($ch, CURLOPT_POST, 1); 
			curl_setopt($ch, CURLOPT_POSTFIELDS, $postdata);
			curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, FALSE);
			curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);
			curl_setopt($curl, CURLOPT_RETURNTRANSFER,true);
			$res = curl_exec($curl);
			curl_close($curl);
			return $res;
		} else return false;
};
$cmd['post'][CMD_FILE]->result=CMD_STRING;
$cmd['post'][CMD_FILE]->visual=Array('title'=>'Загрузить URL через POST','put'=>Array(
		0=>Array('title'=>'URL','type'=>CMD_STRING,'req'=>1),
),'extend_put'=>1);


$cmd['list'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$path=normalize_path(parse_var($op->put[0]));
		if(check_folder($path,'view')){
			if(!empty($op->put[1])) $t=parse_var($op->put[1]);
			else $t=2;
			$files=scan_dir(DOCUMENT_ROOT.'/'.$path,Array(),0,$t);
			if(!empty($files)) foreach($files AS $var=>$name) $files[$var]=str_replace(DOCUMENT_ROOT,'',$name);
			return $files;
		} else return Array();
};
$cmd['list'][CMD_FILE]->result=CMD_ARRAY;
$cmd['list'][CMD_FILE]->result_long=CMD_STRING;
$cmd['list'][CMD_FILE]->visual=Array('title'=>'Список файлов в папке','put'=>Array(
		0=>Array('title'=>'Путь','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Тип','type'=>CMD_STRING,'req'=>0,'list'=>Array(0=>'файлы и папки',1=>'только папки',2=>'только файлы'))
));


$cmd['open'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		$fname=normalize_path(parse_var($op->put));
		$path=dirname($fname);
		if(!empty($_FILES)) foreach($_FILES AS $f){
			if(is_array($f) && $f['name']==$fname){
				return file_get_contents($f['tmp_name']);
			}
		}
		if(!empty($_POST)) foreach($_POST AS $f){
			if(is_array($f) && $f['name']==$fname){
				return file_get_contents($f['tmp_name']);
			}
		}
		if(check_folder($path,'view')){
			if(file_exists(DOCUMENT_ROOT.'/'.$fname)) return file_get_contents(DOCUMENT_ROOT.'/'.$fname);
		}
		return false;
};
$cmd['open'][CMD_FILE]->result=CMD_STRING;
$cmd['open'][CMD_FILE]->visual=Array('title'=>'содержимое файла','put'=>Array(
		0=>Array('title'=>'путь до файла','type'=>CMD_STRING,'req'=>1)
));


$cmd['copy'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);

		$fname_from=normalize_path(parse_var($op->put[0]));
		$path_from=dirname($fname_from);

		$fname_to=normalize_path(parse_var($op->put[1]));
		$path_to=dirname($fname_to);
		
		if(check_folder($path_from,'view') && check_folder($path_to,'edit')){		
			if(!file_exists(DOCUMENT_ROOT.'/'.$fname_from)) return false;
			if(file_exists(DOCUMENT_ROOT.'/'.$fname_to) && (empty($op->put[2]) || !parse_var($op->put[2]))) return false;
			copy(DOCUMENT_ROOT.'/'.$fname_from, DOCUMENT_ROOT.'/'.$fname_to);
			return true;
		}
		return false;
};
$cmd['copy'][CMD_FILE]->result=CMD_NONE;
$cmd['copy'][CMD_FILE]->visual=Array('title'=>'копировать файл','put'=>Array(
		0=>Array('title'=>'путь до файла','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'новый путь','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'заменять, если файл существует','type'=>CMD_LOGICAL,'req'=>0)
));


$cmd['write'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$fname=normalize_path(parse_var($op->put[0]));
		if(!is_dir($fname)) $path=dirname($fname); else $path=$fname;
		if(check_folder($path,'edit')){
			if(is_dir($fname) || !empty($op->put[2])){
				if(!file_exists(DOCUMENT_ROOT.'/'.$fname)){
					check_dir($fname);
				}
				$pref='file';
				$ext='';
				if(!empty($op->put[3])) $ext=parse_var($op->put[3]);
				if(!empty($ext)) $ext='.'.$ext;
				if(!empty($op->put[4])) $pref=parse_var($op->put[4]);
				$type=parse_var($op->put[2]);
				$t=0;
				if($type==1) $t=0;
				if($type==2) $t=1;
				$fname=find_name('/'.$fname.'/',$pref,$ext,0,$t);
			} else $fname='/'.$fname;
			if(!file_exists(DOCUMENT_ROOT.$fname)){
				$f=fopen(DOCUMENT_ROOT.$fname,'w');
				if(!$f) return false;
				fwrite($f,parse_var($op->put[1]));
				fclose($f);
				return $fname;
			}
		}
		return false;
};
$cmd['write'][CMD_FILE]->result=CMD_NONE;
$cmd['write'][CMD_FILE]->visual=Array('title'=>'Записать данные в файл','put'=>Array(
		0=>Array('title'=>'Путь к файлу или до папки','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Данные','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'Тип генерации имени (если указан путь до папки)','type'=>CMD_STRING,'req'=>0,'list'=>Array('1'=>'Генерировать имя по номеру','2'=>'Генерировать имя случайным образом')),
		3=>Array('title'=>'Расширение/постфикс имени файла','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'Префикс имени файла','type'=>CMD_STRING,'req'=>0)
));


$cmd['mkdir'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		$fname=normalize_path(parse_var($op->put));
		$path=dirname($fname);
		if(check_folder($path,'edit')){
			check_dir(DOCUMENT_ROOT.'/'.$fname);
		}
		return false;
};
$cmd['mkdir'][CMD_FILE]->result=CMD_NONE;
$cmd['mkdir'][CMD_FILE]->visual=Array('title'=>'Создать папку','put'=>Array(
		0=>Array('title'=>'Путь до новой папки','type'=>CMD_STRING,'req'=>1)
));


$cmd['del'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		$fname=normalize_path(parse_var($op->put));
		$path=dirname($fname);
		if(check_folder($path,'del')){
			if(file_exists(DOCUMENT_ROOT.'/'.$fname)){
				unlink(DOCUMENT_ROOT.'/'.$fname);
				return false;
			}
		}
		return true;
};
$cmd['del'][CMD_FILE]->result=CMD_NONE;
$cmd['del'][CMD_FILE]->visual=Array('title'=>'Удалить файл или папку','put'=>Array(
		0=>Array('title'=>'Путь до папки или файла','type'=>CMD_STRING,'req'=>1)
));


$cmd['clean'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		$path=normalize_path(parse_var($op->put));
		if(check_folder($path,'del')){
			$t=2;
			$files=scan_dir(DOCUMENT_ROOT.'/'.$path,Array(),0,$t);
			if(!empty($files)) foreach($files AS $var=>$name){
				unlink($name);
			}
			return false;
		}
		return false;
};
$cmd['clean'][CMD_FILE]->result=CMD_NONE;
$cmd['clean'][CMD_FILE]->visual=Array('title'=>'Очистить папку от файлов','put'=>Array(
		0=>Array('title'=>'Путь до папки','type'=>CMD_STRING,'req'=>1)
));


$cmd['move'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		$path1=normalize_path(parse_var($op->put[0]));
		$path2=normalize_path(parse_var($op->put[1]));
		$idir1=is_dir(DOCUMENT_ROOT.'/'.$path1);
		$idir2=is_dir(DOCUMENT_ROOT.'/'.$path2);
		if(!$idir1) $dir1=dirname($path1); else $dir1=$path1;
		if(!$idir2) $dir2=dirname($path2); else $dir2=$path2;
		if(!$idir1 && !$idir2){
			//переименование файлов
			if(check_folder($dir1,'view') && check_folder($dir2,'edit') && !file_exists(DOCUMENT_ROOT.'/'.$path2)){
				rename(DOCUMENT_ROOT.'/'.$path1,DOCUMENT_ROOT.'/'.$path2);
				return false;
			}
		}
		if(!$idir && $idir2){
			//перенос файла в другую папку
			if(check_folder($dir1,'view') && check_folder($dir2,'edit') && !file_exists(DOCUMENT_ROOT.'/'.$path2.'/'.basename($path1))){
				rename(DOCUMENT_ROOT.'/'.$path1,DOCUMENT_ROOT.'/'.$path2.'/'.basename($path1));
				return false;
			}
		}
		if($idir && !$idir2){
			//переименование папки
			$dir1=dirname($dir1);
			$dir2=dirname($dir2);
			if(check_folder($dir1,'view') && check_folder($dir2,'edit') && !file_exists(DOCUMENT_ROOT.'/'.$path2)){
				rename(DOCUMENT_ROOT.'/'.$path1,DOCUMENT_ROOT.'/'.$path2);
				return false;
			}
		}
		if($idir && $idir2){
			//перенос папки из одного места в другое
			$dir1=basename($dir1);
			if(check_folder($dir1,'view') && check_folder($dir2,'edit') && !file_exists(DOCUMENT_ROOT.'/'.$path2.'/'.basename($dir1))){
				rename(DOCUMENT_ROOT.'/'.$path1,DOCUMENT_ROOT.'/'.$path2.'/'.basename($dir1));
				return false;
			}
		}
};
$cmd['move'][CMD_FILE]->result=CMD_NONE;
$cmd['move'][CMD_FILE]->visual=Array('title'=>'переместить файл или папку','put'=>Array(
		0=>Array('title'=>'Путь исходной папки или файла','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Путь конеченой папки','type'=>CMD_STRING,'req'=>1)
));


$cmd['exists'][CMD_FILE]->process=function(&$data,&$op,&$ops){
		$path=normalize_path(parse_var($op->put));
		return file_exists($path);
};
$cmd['exists'][CMD_FILE]->result=CMD_STRING;
$cmd['exists'][CMD_FILE]->visual=Array('title'=>'Наличие файла','put'=>Array(
		0=>Array('title'=>'Путь до файла','type'=>CMD_STRING,'req'=>1)
));


?>