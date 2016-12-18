<?php

include_once('lexer.inc');
include_once('prepend.inc');
include_once('support.inc');
include_once('shell.inc');
include_once('parse_var.inc');

// Разрешённые PHP функции (доступны через [#name_function()] )

$func=Array('shell_tpl','shell_part','shell_module','dblchar','count','empty','rand','in_array','array_flip','strlen','substr','str_replace','isset','htmlspecialchars','clone_file','preview','strpos','header','urlencode','date','get_tag','get_all_tag','get_tagA','get_all_tag3','explode','clean_html','del_tag','latinica','ucfirst','strtolower','strtoupper','trim','md5','win_utf8','utf8_win','simple_mail','check_url','chr','ord','get_code','get_code2','get_code3','is_rus','rotate');

// опции шаблонизатора

$allow_components=Array();	// разрешённые пути компонентов
						// editor - /core/editor/components/  (включается перед специальным выводом шаблона)
						
$tpl_register_globals=false;	// возможность обработки глобальных переменных в шаблонизаторе

// Цикл

function do_for($tpl,$start,$end,$blocks=Array()){
	stf('do_for');
	$i=0;
	$result='';
	$res=backup_globals();
	$GLOBALS["for_min"]=$start;
	$GLOBALS["for_max"]=$end;
	$s=0;


	if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
	if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;//SPEC STEP 2 сделан для того, чтобы он не обнулялся при переходе к новой части
	$GLOBALS["spec_step"]++;//я долго думал почему тут надо ставить это условие
	$GLOBALS["spec_step2"]++;//я долго думал почему тут надо ставить это условие

	if($start<=$end){
		for($i=$start;$i<=$end;$i++){
			$s++;
			$GLOBALS["index"]=$i;
			$GLOBALS["step"]=$s;
			$GLOBALS["break"]=0;
			$GLOBALS["continue"]=0;
			
			$GLOBALS["up3"][$GLOBALS["spec_step"]]=$i;
			$GLOBALS["up4"][$GLOBALS["spec_step"]]=$i;
			$GLOBALS["up5"][$GLOBALS["spec_step"]]=$i;
			
			$result2=shell_tpl($tpl,1,$blocks);
			$result=concat_result($result,$result2);
			//if(!empty($GLOBALS["clear"])) $result=$result2; else $result.=$result2;
			if(!empty($GLOBALS["exit"])){return_globals($res);etf('do_for'); return $result;}
			if(!empty($GLOBALS["break"])){ /*return_globals($res);*/ /*возможно тут и в continue и ниже в break и continue нужно воткнуть return globals*/ $GLOBALS["break"]=false;  break; }
			if(!empty($GLOBALS["continue"])){ /*return_globals($res);*/ if($GLOBALS['continue']>1) $i=$i+$GLOBALS['continue']-1; $GLOBALS["continue"]=false; continue; }
		}
	} else {
		for($i=$start;$i>=$end;$i--){
			$s++;
			$GLOBALS["index"]=$i;
			$GLOBALS["step"]=$s;
			$GLOBALS["break"]=0;
			$GLOBALS["continue"]=0;
			
			$GLOBALS["up3"][$GLOBALS["spec_step"]]=$i;
			$GLOBALS["up4"][$GLOBALS["spec_step"]]=$i;
			$GLOBALS["up5"][$GLOBALS["spec_step"]]=$i;
			
			$result2=shell_tpl($tpl,1,$blocks);
			$result=concat_result($result,$result2);
			//if(!empty($GLOBALS["clear"])) $result=$result2; else $result.=$result2;
			if(!empty($GLOBALS["exit"])){return_globals($res);etf('do_for'); return $result;}
			if(!empty($GLOBALS["break"])){ /*return_globals($res);*/ $GLOBALS["break"]=false;	break; }
			if(!empty($GLOBALS["continue"])){ /*return_globals($res);*/  if($GLOBALS['continue']>1) $i=$i-$GLOBALS['continue']+1; $GLOBALS["continue"]=false; continue; }
		}
	}
	return_globals($res);
	etf('do_for');
	return $result;
}

function do_tree($tpl,$tree,$after='',$reverse=0,$linear=0,$step=0,$for_last=0,$blocks=Array()){//for_lat - для определения последнего прогона цикла
	stf('do_tree');
	$i=0;
	$result='';
	$GLOBALS["in_tree"]=1;
	if(!empty($tree->rows)) $tree=$tree->rows;
	if($reverse==1 && !empty($tree)) krsort($tree);
	if($reverse==2 && !empty($tree)) {srand(); shuffle($tree);}
	$first=1;
	$first2=1;
	$res=backup_globals();
	if(!empty($tree) && !isset($tree->rows) && is_array($tree)){
		$GLOBALS["count"]=count($tree);
		if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;//SPEC STEP 2 сделан для того, чтобы он не обнулялся при переходе к новой части
		if($step==0) $GLOBALS["spec_step"]++;//я долго думал почему тут надо ставить это условие
		if($step==0) $GLOBALS["spec_step2"]++;//я долго думал почему тут надо ставить это условие
		$special_skip=0;
		foreach($tree AS $var=>$tre){
			if($special_skip>0){
				$i++;
				$special_skip--;
				continue;
			}
			//$res=backup_globals();
			$GLOBALS["step"]=$step;
			$GLOBALS["pass"]=0;
			$GLOBALS["break"]=0;
			$GLOBALS["continue"]=0;
			/*if(!is_object($tre) && !empty($GLOBALS["cur_type"]) && $GLOBALS["cur_type"]=='row'){
				global $rlink;
				seek_rlink($tre);
				if(isset($rlink[$tre])) $tre=$rlink[$tre];
			}*/
			if(is_object($tre)){
				if(empty($GLOBALS["cur_type"])) $GLOBALS["cur_type"]='';
				if(!empty($tre->table)) $GLOBALS["cur_table"]=$tre->table;			
				$GLOBALS["current"]=$tre->id;
				if(!empty($tre->type)){
					$GLOBALS["cur_type"]=$tre->type;
				} else $GLOBALS["cur_type"]='row';
				$GLOBALS["up"][$GLOBALS["spec_step"]]=$tre->id;
				if(!empty($tre->type)) $GLOBALS["up2"][$GLOBALS["spec_step"]]=$tre->type;
				else {
					if(isset($tre->id)) $GLOBALS["up2"][$GLOBALS["spec_step"]]='row';
					else $GLOBALS["up2"][$GLOBALS["spec_step"]]='';
				}
			}// else $GLOBALS["current"]=$tre; //опасность
			$GLOBALS["up3"][$GLOBALS["spec_step"]]=$var;
			$GLOBALS["up4"][$GLOBALS["spec_step"]]=$i;
			$GLOBALS["up5"][$GLOBALS["spec_step"]]=$tre;
			
			$GLOBALS["index"]=$i;
			$GLOBALS["cur_var"]=$var;
			$GLOBALS["for_value"]=$tre;
			$GLOBALS["last"]=0;$slast=0;
			if($i==/*count($tree)*/$GLOBALS["count"]-1){
				if((empty($tre->sub) || $GLOBALS["pass"]!=0 /*  <-- тут PASS то ещё не определился. по идее его надо вырезать gettagA с исключением [tree [/tree] */) && ($step==0 || $for_last!=0)) $GLOBALS["last"]=1;
			}
	
			if($first){
				global $tree_vars,$tv_cache;
				$tree_vars[$GLOBALS["spec_step2"]]=Array();
			}
			if($first2){
				global $tree_vars2;
				$tree_vars2[$GLOBALS["spec_step2"]]=Array();
			}
			$result2=shell_tpl($tpl,1,$blocks);
	
			if($first && !$GLOBALS["no_cache"]){//оптимизация вытаскивания значений (нужно как-то защитить от повторных вытаскиваний)
				//if(count($tree)*count($tree_vars[$GLOBALS["spec_step2"]])<12) $first=false;//чтобы не возиться с мелочью
				//else
					if(tree_optimize($tre,$tree)) $first=false;
			}
			
			//Кеширование наподобие tree_optimize, только имеет отношение к выборке объектов
			if($first2 && !empty($tree_vars2[$GLOBALS["spec_step2"]]) && !$GLOBALS["no_cache"]){
				$ids=Array();
				foreach($tree AS $tre2) $ids[$tre2->id]=$tre2->id;
				$ids=implode(',',$ids);
				if(!empty($ids)) foreach($tree_vars2[$GLOBALS["spec_step2"]] AS $tv){
					$tv=explode('.',$tv);
					$tv_table=$tv[0];
					$tv_seek_ena=$tv[1];
					$tv_seek_ex=$tv[2];
					$tv_ex=$tv[3];
					$tv_tex=$tv[4];
					$tv_ignore_table=$tv[5];
					$tv_seek_auth=$tv[6];
	
					//выдёргиваем три случайных элемента цикла и если все они есть в базе, то кеширование не проводим
					global $gsres;
					$bool=true;
					$xtrid=0;
					while($xtrid<=3){
						$trid=$tree[array_rand($tree)]->id;
						if(!isset($gsres[$trid][$tv_table][$tv_seek_ena][$tv_seek_ex][$tv_ex][$tv_tex][$tv_ignore_table][$tv_seek_auth])){
							$bool=false;
							break;
						}
						$xtrid++;
					}
					if(!$bool) get_sub($ids,$tv_table,$tv_seek_ena,$tv_seek_ex,$tv_ignore_table,0,0,$tv_tex,$tv_ex,$tv_table,0,$tv_table,$tv_seek_auth);
				}
				$first2=false;
			}
			
			if($i==/*count($tree)*/$GLOBALS["count"]-1){		
				if(!empty($tre->sub) && $GLOBALS["pass"]==0) $slast=1;
			}
		
			$result=concat_result($result,$result2);
			//if(!empty($GLOBALS["clear"])) $result=$result2; else $result.=$result2;
			if(!empty($GLOBALS["exit"])){ return_globals($res); etf('do_tree'); return $result;}
			if(!empty($GLOBALS["break"])){	/*return_globals($res);*/ $GLOBALS["break"]=false; break; }
			if(!empty($GLOBALS["continue"])){  /*return_globals($res);*/ $i++;  if($GLOBALS['continue']>1) $special_skip=$GLOBALS['continue']-1; $GLOBALS["continue"]=false; continue; }
			//return_globals($res); //раньше этот ретурнс был после последующего do_tree (что естественно), почему-то так заработало
			if(!empty($tre->sub) && $GLOBALS["pass"]==0 && ($linear==0 || ($linear>0 && $step<$linear-1))) $result.=do_tree($tpl, $tre->sub, $after, $reverse, $linear, $step+1, $slast);
			//return_globals($res); //раньше этот ретурнс был после последующего do_tree (что естественно), почему-то так заработало
			$GLOBALS["pass"]=0;
			if(!empty($after)){
				/*$res=backup_globals();*/
				$GLOBALS["step"]=$step;
				$GLOBALS["pass"]=0;
				$GLOBALS["break"]=0;
				$GLOBALS["continue"]=0;
				//if(empty($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
				//if(empty($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
				//if($step==0) $GLOBALS["spec_step"]++;
				//if($step==0) $GLOBALS["spec_step2"]++;
				/*
				if(is_object($tre)){
					if(empty($GLOBALS["cur_type"])) $GLOBALS["cur_type"]='';
					if(!empty($tre->table)) $GLOBALS["cur_table"]=$tre->table;
					$GLOBALS["current"]=$tre->id;
					if(!empty($tre->type)) $GLOBALS["cur_type"]=$tre->type; else $GLOBALS["cur_type"]='';
					$GLOBALS["up"][$GLOBALS["spec_step"]]=$tre->id;
					if(!empty($tre->type)) $GLOBALS["up2"][$GLOBALS["spec_step"]]=$tre->type;
					else $GLOBALS["up2"][$GLOBALS["spec_step"]]='';
				}
				*/
				$result2=shell_tpl($after,1,$blocks);
				$result=concat_result($result,$result2);
				//if(!empty($GLOBALS["clear"])) $result=$result2; else $result.=$result2;
				if(!empty($GLOBALS["exit"])){return_globals($res); /*$GLOBALS["spec_step"]--; */etf('do_tree'); return $result;}
				if(!empty($GLOBALS["break"])){	/*return_globals($res);*/ $GLOBALS["break"]=false; /*$GLOBALS["spec_step"]--;*/ break; }
				if(!empty($GLOBALS["continue"])){  /*return_globals($res);*/ $i++; if($GLOBALS['continue']>1) $special_skip=$GLOBALS['continue']-1; $GLOBALS["continue"]=false; /*$GLOBALS["spec_step"]--; */continue; }
				//return_globals($res);
			}
			//$GLOBALS["spec_step"]--;
			$i++;
		}
	}
	return_globals($res);
	etf('do_tree');
	$GLOBALS["in_tree"]=0;
	return $result;
}

function do_if($condition){
	global $last_condition;
	if(empty($condition) && !empty($last_condition)) $condition=$last_condition;
	else $last_condition=$condition;
	return parse_var($condition);
}

function end_tpl(/*$tpl*/$um,$res,$lim=0,$force_cache=0){
	global $tpl_step,$ptpl,$utpl,$cache_tpl,$puid,$user,$zone_url,$tpl_static,$tpl_cookies,$tpl_time;
	if(!$cache_tpl && empty($force_cache)) return '';
	if(!empty($_POST)) return '';
	if(!empty($tpl_time) && !empty($lim) && $lim>$tpl_time) $lim=$tpl_time;
	if(empty($lim) && !empty($tpl_time)) $lim=$tpl_time;
	$cache_user=$user->id;
	$autodel=true;
	$use_host=false;
	if(!empty($force_cache) && $force_cache[0]=='!'){
		$use_host=true;
		$force_cache=substr($force_cache,1);
	}
	if(!empty($force_cache) && $force_cache[0]=='@'){
		$autodel=false;
		$force_cache=substr($force_cache,1);
	}
	if($force_cache<0){
		$cache_user='';
		$force_cache=-$force_cache;
	}
	if($use_host){
		$hhost=$GLOBALS['host'];
	} else $hhost='';
	stf('end_tpl');
	$tpl_step--;
	if($tpl_step==0 || $force_cache!=0){
		//$um=md5($tpl);
		
		$cookcache='';
		$cookcache2='';
		if(!empty($tpl_cookies) && $force_cache!=2) foreach($_COOKIE AS $var=>$value) if($var!='user_hook' && $var!='user_login' && isset($tpl_cookies[$var])){
			if(!empty($cookcache2)) $cookcache2.=',';
			$cookcache2.=$var;
			if(!is_array($value)) $cookcache.='&'.$var.'='.$value; else $cookcache.='&'.$var.'='.implode(',',$value);
		}
		
		if(!file_exists(FTEMP)){ mkdir(FTEMP); chmod(FTEMP,DEF_DRMOD);}
		if(!file_exists(FTEMP.'tpl')){ mkdir(FTEMP.'tpl');  chmod(FTEMP.'tpl',DEF_DRMOD);}
		if(!file_exists(FTEMP.'tpl/row')){ mkdir(FTEMP.'tpl/row');  chmod(FTEMP.'tpl/row',DEF_DRMOD);};
		if(!file_exists(FTEMP.'tpl/rows')){ mkdir(FTEMP.'tpl/rows');  chmod(FTEMP.'tpl/rows',DEF_DRMOD); }
		if(!file_exists(FTEMP.'tpl/part')){ mkdir(FTEMP.'tpl/part');  chmod(FTEMP.'tpl/part',DEF_DRMOD); }
		global $cur_var,$up;
		$ut='';
		if(!empty($up) && is_array($up)) foreach($up AS $var=>$value) $ut.=$var.'.'.$value.';';
		if($force_cache==2) $um2=md5(/*$puid.*/$GLOBALS["cur_ex"].$cache_user.$hhost);//Убрать пуйд нужно было потому что он по сути содержит в себе переменные гет, а их то тут и надо проигнорировать как раз. может сделать другой пуйд без гет переменных...
		else if($force_cache==3){
			$um2b=md5($puid.$GLOBALS["cur_ex"].$cache_user.$hhost);
			$um2=md5($puid.$cookcache.$GLOBALS["cur_ex"].$cache_user.$hhost);//сатарая добрая пуйда
		} else if($force_cache==4){
			$um2b=md5($puid.$GLOBALS["cur_ex"].$GLOBALS["cur_row"].$cur_var.$ut.$cache_user.$hhost);
			$um2=md5($puid.$cookcache.$GLOBALS["cur_ex"].$GLOBALS["cur_row"].$cur_var.$ut.$cache_user.$hhost);
		} else if($force_cache==5){
			$um2b=md5($ut.$cache_user.$hhost);
			$um2=md5($ut.$cache_user.$hhost);
		}
		else {
			//$um2b=md5(str_replace($zone_url,'',$GLOBALS["sforward"]).$puid.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost);
			//$um2=md5(str_replace($zone_url,'',$GLOBALS["sforward"]).$puid.$cookcache.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost);//вставка тут cur_var под бАААльшим сомнением, так например одинаковые блоки на одном сайте будут кешированы в разных файлах (ну и что? всё равно же к урлу привязано хотя не должно быть... можно отдельно сделать какойнить ^* без привязки к урлу и cur-var)
			$um2b=md5($GLOBALS["sforward"].$puid.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost);
			$um2=md5($GLOBALS["sforward"].$puid.$cookcache.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost);//вставка тут cur_var под бАААльшим сомнением, так например одинаковые блоки на одном сайте будут кешированы в разных файлах (ну и что? всё равно же к урлу привязано хотя не должно быть... можно отдельно сделать какойнить ^* без привязки к урлу и cur-var)			
		}
		if(!file_exists(FTEMP.'tpl/'.$um)){ mkdir(FTEMP.'tpl/'.$um); chmod(FTEMP.'tpl/'.$um,DEF_DRMOD); }
		$dir=FTEMP.'tpl/'.$um.'/';
		$f=fopen($dir.$um2,'w');
		if(empty($lim) || $lim[0]!='$') $d=get_min(date('Y-m-d H:i'));			
		else $d=get_day(date('Y-m-d'));
		fwrite($f,$d.'^'.$lim.'|');
		//if(!empty($cookcache2[$um])) fwrite($f,'^'.$cookcache2[$um]);
		//fwrite($f,'|');
		fwrite($f,$res);
		fclose($f);
		if(!empty($cookcache2) && $force_cache!=2){
			$f=fopen($dir.$um2b.'!','w');
			fwrite($f,$cookcache2);
			fclose($f);
		}
		if(!empty($tpl_static)){
			$f=fopen($dir.$um2.'_static','w');
			fwrite($f,$tpl_static);//тут по идее надо обнулять tpl_static но тогда вышестоящее кеширование схавает эту фичу
			//echo $tpl_static.'<br>'.$res.'<hr>';
			//$tpl_static='';//попробуем обнулить. мне кажется из-за этого косяки прут на едди.ант-дизайне//не помогло
			fclose($f);	
		}
		//row	row_id
		//rows	table_id
		//part	part_id
		if($autodel){
			if(!empty($utpl["row"])) foreach($utpl["row"] AS $var=>$value){
				$f=fopen(FTEMP.'tpl/row/'.$var,'a');
				if(filesize(FTEMP.'tpl/row/'.$var)!=0) fwrite($f,'*');
				fwrite($f,$um.'/'.$um2);
				fclose($f);
			}
			if(!empty($utpl["rows"])) foreach($utpl["rows"] AS $var=>$value){
				$f=fopen(FTEMP.'tpl/rows/'.$var,'a');
				if(filesize(FTEMP.'tpl/rows/'.$var)!=0) fwrite($f,'*');
				fwrite($f,$um.'/'.$um2);
				fclose($f);
			}
			if(!empty($utpl["part"])) foreach($utpl["part"] AS $var=>$value){
				$f=fopen(FTEMP.'tpl/part/'.$var,'a');
				if(filesize(FTEMP.'tpl/part/'.$var)!=0) fwrite($f,'*');
				fwrite($f,$um);
				fclose($f);
			}
		}
	}
	etf('end_tpl');
}

function start_tpl($um,/*$limit=0,*/$force_cache=1){
	global $puid,$user;
	global $cur_var,$up,$zone_url,$tpl_cookies;
	stf('start_tpl');
	
	//$um=md5($tpl);
	$ut='';
	$cache_user=$user->id;
	$use_host=false;
	if(!empty($force_cache) && $force_cache[0]=='!'){
		$use_host=true;
		$force_cache=substr($force_cache,1);
	}
	if(!empty($force_cache) && $force_cache[0]=='@') $force_cache=substr($force_cache,1);
	if($force_cache<0){
		$cache_user='';
		$force_cache=-$force_cache;
	}
	if($use_host){
		$hhost=$GLOBALS['host'];
	} else $hhost='';
	if(!empty($up) && is_array($up)) foreach($up AS $var=>$value) $ut.=$var.'.'.$value.';';	
	//подгружаем используемые шаблоном куки (если разные кеши для разного адреса или окружения, то и файл используемых кук тоже будет разный, по этому тут также генерится um2 только без куки (т.к. он ещё неизвестен)
	$cookcache='';
	//if($GLOBALS['cur_module']==21 && $GLOBALS['cur_part']==262){
	//	echo '<br>'.$puid.'<br>';
	//}	
	if($force_cache!=2){
		if(!isset($GLOBALS["cur_row"])) $GLOBALS["cur_row"]=0;
		if(!isset($GLOBALS["cur_ex"])) $GLOBALS["cur_ex"]=0;
		if($force_cache==3) $um2=md5($puid.$GLOBALS["cur_ex"].$cache_user.$hhost);
		else if($force_cache==4) $um2=md5($puid.$GLOBALS["cur_ex"].$GLOBALS["cur_row"].$cur_var.$ut.$cache_user.$hhost);
		else $um2=md5($GLOBALS["sforward"].$puid.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost); 
		//else $um2=md5(str_replace($zone_url,'',$GLOBALS["sforward"]).$puid.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost); 
		$ur=FTEMP.'tpl/'.$um.'/'.$um2;
		if(file_exists($ur.'!')){	
			$f=file_get_contents($ur.'!');
			$f=explode(',',$f);
			$f2=Array();
			if(!empty($f)) foreach($f AS $v) $f2[$v]=1;
			foreach($_COOKIE AS $var=>$value) if($var!='user_hook' && $var!='user_login' && isset($f2[$var])){
				$tpl_cookies[$var]=1;
				if(!is_array($value)) $cookcache.='&'.$var.'='.$value; else $cookcache.='&'.$var.'='.implode(',',$value);
			}
		}
	}
	if($force_cache==2) $um2=md5($GLOBALS["cur_ex"].$cache_user.$hhost);
	else if($force_cache==3) $um2=md5($puid.$cookcache.$GLOBALS["cur_ex"].$cache_user.$hhost);
	else if($force_cache==4) $um2=md5($puid.$cookcache.$GLOBALS["cur_ex"].$GLOBALS["cur_row"].$cur_var.$ut.$cache_user.$hhost);
	else if($force_cache==5) $um2=md5($ut.$cache_user.$hhost);
	else $um2=md5($GLOBALS["sforward"].$puid.$cookcache.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost); //добавил сюда $puid и $GLOBALS["cur_row"]
	//else $um2=md5(str_replace($zone_url,'',$GLOBALS["sforward"]).$puid.$cookcache.$GLOBALS["cur_row"].$GLOBALS["cur_ex"].$cur_var.$ut.$cache_user.$hhost); //добавил сюда $puid и $GLOBALS["cur_row"]
	$ur=FTEMP.'tpl/'.$um.'/'.$um2;
	if(file_exists($ur)){
		$f=file_get_contents($ur);
		$m='';$l=0;$b=false;$m2='';
		for($i=0;$i<strlen($f);$i++){
			if($f[$i]=='|'){
				if($m2=='') $m2=0;
				if($m2[0]!='$'){
					$limit=$m2;
					$l=get_min(date('Y-m-d H:i'))-$m;
				} else {
					$limit=substr($m2,1);
					$l=get_day(date('Y-m-d'))-$m;
				}
				if($limit!=0 && $l>=$limit){ etf('start_tpl'); return '';}
				else{$f=substr($f,$i+1,strlen($f)-$i-1);}
				break;
			} else if($f[$i]=='^')$b=true; else if(!$b) $m.=$f[$i]; else $m2.=$f[$i];
		}
		if(file_exists($ur.'_static')){
			//echo '!'.implode('',file($ur)).'!<br><br>
			//';
			$f.=shell_tpl(implode('',file($ur.'_static')));
		}
		etf('start_tpl');
		return $f;
	} else {
		etf('start_tpl');
		return '';
	}
	etf('start_tpl');
}

function del_url_cache($type,$id){
	if($type=='row'){
		$p=FTEMP.'url/rows/'.$id;
		$d=FTEMP.'url/';
		if(file_exists($p)){
			$files=explode(',',file_get_contents($p));
			foreach($files AS $f)if(file_exists($d.$f)){
				unlink($d.$f);
			}
			unlink($p);
		}
	}
	if($type=='part'){
		if(file_exists(FTEMP.'url/'.$id)) rmdir2(FTEMP.'url/'.$id);
	}
}

function del_cache($type,$id){
	if($type=='row' || $type=='part'){
		del_url_cache($type,$id);
	}
	$fl=FTEMP.'tpl/'.$type.'/'.$id;
	if(!file_exists($fl)) return '';
	$fs=filesize($fl);
	if($fs==0) return '';
	$f=fopen($fl,'r');
	$tmp=fread($f,$fs);
	fclose($f);
	$tmp=explode('*',$tmp);
	if(!empty($tmp)) foreach($tmp AS $tm){
		if(file_exists(FTEMP.'tpl/'.$tm)){
			if($type=='part') rmdir2(FTEMP.'tpl/'.$tm);
			else @unlink(FTEMP.'tpl/'.$tm);
		}
	}
	if(file_exists($fl)) @unlink($fl);
}

function get_cache_conf(){
	global $tpl_time,$tpl_url,$tpl_param,$tpl_cookies;
	$r->time=$tpl_time;
	$r->url=$tpl_url;
	$r->param=$tpl_param;
	$r->cookies=$tpl_cookies;
	unset($tpl_time);
	$tpl_url=0;
	$tpl_param=0;
	$tpl_cookies=Array();
	return $r;
}

function set_cache_conf($conf){
	global $tpl_time,$tpl_url,$tpl_param,$tpl_cookies;
	if(empty($conf->time)) $conf->time=0;
	if(empty($tpl_time)) $tpl_time=$conf->time;
	else if($conf->time>$tpl_time || empty($conf->time)/*??*/) $conf->time=$tpl_time;
	if(!empty($tpl_url)) $conf->url=1;
	if(!empty($tpl_param)) $conf->param=1;
	if(!empty($tpl_cookies)) foreach($tpl_cookies AS $var=>$value) $conf->cookies[$var]=1;
	$tpl_time=$conf->time;
	$tpl_url=$conf->url;
	$tpl_param=$conf->param;
	$tpl_cookies=$conf->cookies;
	return $conf;
}

function shell_tpl($tpl,$cache=1,$blocks=Array(),$level=0){
	global $tpl_step, $cache_tpl, $tpl_url, $tpl_param, $tpl_cookies, $tpl_time;
	
	// Проверка выхода
	if(!empty($GLOBALS["xbreak"])) return '';
	if(empty($GLOBALS["lvl2"])) $GLOBALS["lvl2"]=0;
	$GLOBALS["lvl2"]++;
	if(!empty($GLOBALS["exit"])) return '';
	
	// Парсинг
	if(!is_object($tpl)) $t=start_parse_tpl($tpl,0,!$GLOBALS["cache_tpl2"]);
	else $t=$tpl;
	
	//echo '<pre>';
	//var_dump($t);
	//echo '</pre>';
	//exit;
	
	//if($GLOBALS['user']->ip=='212.164.230.75') var_dump($tpl);
	
	// Кеширование
	$force_cache=0;
	$cachable=isset($t->cache);
	$cache_user=true;
	$autodel=true;
	$use_host=false;
	$use_tpl=false;
	$use_tpl_type=0;
	$use_tpl_body='';
	$lim=0;
	$cache_md5='';
	if(!isset($t->cache)){
		/*$t->cache='';*/
	} else {
		global $cur_part;
		if(isset($t->cache_md5)){
			$cache_md5=$t->cache_md5.$cur_part;
		} else {
			$cache_md5=md5(serialize($t)).$cur_part;
		}
		//добавление cur_part нужно для правильной трактовки вложенных шаблонов
		if(isset($t->cache_ignore_user)) $cache_user=false;
		if(isset($t->cache_no_del)) $autodel=false;
		if(isset($t->cache_host)) $use_host=true;
		if(isset($t->cache_time)){
			$lim=$t->cache_time;
			if(isset($t->cache_in_day)) $lim='$'.$lim;
		}
	}
	if($cachable && ($t->cache=='&' || $tpl_url)){
		$force_cache=1;
		$cache=1;
		$tpl_param=1;
		$tpl_url=1;
		$cachable=false;
	}
	if(($cachable && $t->cache=='#') || ($cachable && $t->cache!='%' && ($tpl_param || !empty($tpl_cookies)))){
		$cachable=false;
		$cache=1;
		$force_cache=3;
		$tpl_param=1;
	}
	if(($cachable && $t->cache=='%') || ($cachable && ($tpl_param || !empty($tpl_cookies)))){
		$cachable=false;
		$cache=1;
		$force_cache=4;
		$tpl_param=1;
	}
	if($cachable && $t->cache=='_'){
		$cache=1;
		$force_cache=5;
	}
	if($cachable && $t->cache=='*'){
		$cache=1;
		$force_cache=2;
	}
	if(!empty($lim)) $tpl_time=$lim;
	if(!isset($tpl_step)){
		$tpl_step=1;
	} else $tpl_step++;
	if(!$cache_user) $force_cache=-$force_cache;
	if(!$autodel) $force_cache='@'.$force_cache;
	if($use_host) $force_cache='!'.$force_cache;
	if($cache==1 && ($cache_tpl || !empty($force_cache)) && empty($_POST)){
		if(empty($cache_md5)){
			if(!is_object($tpl)) $cache_md5=md5($tpl); else $cache_md5=md5(serialize($tpl));
		}
		$st=start_tpl($cache_md5,/*$lim,*/$force_cache);
		if(!empty($st)){
			$GLOBALS["lvl2"]--;
			$tpl_step--;
			return $st;
		}
	}

	$res='';
	if(!empty($t->elems)) foreach($t->elems AS $e){
		if($e->type==TPL_STATIC && !$use_tpl){
			$res.=$e->data;
		}
		if($e->type==TPL_DYNAMIC){
			if($e->dynamic->type==TPL_PROC){				
				if($e->dynamic->main=='use'){					
					unset($res);
					$res='';
					$use_tpl_type=$e->dynamic->ops[1]->data;
					$use_tpl_body='';
					if(!empty($e->dynamic->ops[2])){
						$x=parse_var($e->dynamic->ops[2]);
					}
					if($use_tpl_type=='module' || $use_tpl_type=='mod'){
						global $cur_module;
						if(!empty($cur_module)) $use_tpl_body=getrowval('SELECT part_body FROM main_part WHERE part_module='.$cur_module.' AND part_type!=2 AND part_sname=\''.safe_sql_input($x).'\'','part_body');
					}
					if($use_tpl_type=='up'){
						global $cur_module;
						if(!empty($cur_module) && !empty($GLOBALS["mod_part"][$cur_module])){
							$x=$GLOBALS["mod_part"][$cur_module];
							$x=getrowval('SELECT part_owner FROM main_part WHERE part_id='.$x,'part_owner');
							if(!empty($x)) $use_tpl_body=getrowval('SELECT part_body FROM main_part WHERE part_id='.$x,'part_body');
						}
					}
					if($use_tpl_type=='tpl'){
						$use_tpl_body=$x;
					}
					if($use_tpl_type=='func' || $use_tpl_type=='function' || $use_tpl_type=='component' || $use_tpl_type=='show' || $use_tpl_type=='form'){
						$proc_type=0;
						if($use_tpl_type=='func' || $use_tpl_type=='function') $proc_type=0;
						if($use_tpl_type=='show') $proc_type=1;
						if($use_tpl_type=='component') $proc_type=2;
						if($use_tpl_type=='form') $proc_type=3;
						$use_tpl_body=getrowval('SELECT part_body FROM main_part WHERE part_type=2 AND part_proc='.$proc_type.' AND part_sname=\''.safe_sql_input($x).'\'','part_body');
					}
					if($use_tpl_body) $use_tpl=true;
				} else {
					$res2=parse_var($e->dynamic->ops);
					$res=concat_result($res,$res2,($level==0));
					/*if((is_array($res2) || is_object($res2)) && !empty($GLOBALS["clear"])) $res=$res2;
					else if(is_object($res2) && isset($res2->id)) $res2=$res2->id;
					if(!is_array($res2) && !is_object($res2)){
						if(!empty($GLOBALS["clear"])) $res=$res2; else $res.=$res2;
					}*/
					/*if(strstr(serialize($e->dynamic->ops),'auto')){
						$GLOBALS["clear"]=false;
						$GLOBALS["exit"]=false;
					}*/
					
					if(!empty($GLOBALS["exit"])){ break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}
					if(!empty($GLOBALS["break"])) { break;/*end_tpl($cache_md5,$res,$lim); return $res;*/}
					if(!empty($GLOBALS["continue"])) {break;/*end_tpl($cache_md5,$res,$lim); return $res;*/}
					if(!empty($GLOBALS["xbreak"])){break;/*end_tpl($cache_md5,$res,$lim); return $res;*/}
				}
			}
			// Новые типы обработчиков нужно размещать под TPL_BLOCK (под проверкой USE)
			if($e->dynamic->type==TPL_BLOCK){
				$name='';
				$silent=false;
				if(!empty($e->dynamic->ops)) foreach($e->dynamic->ops AS $dops){
					if($dops->type==LEXER_SUB && count($dops->sub)==2 && $dops->sub[0]->next_operation=='='){
						$dop_var=$dops->sub[0]->data;
						$dop_val=parse_var($dops->sub[1]);
						if($dop_var=='name') $name=$dop_val;
					}
					if(isset($dops->data) && $dops->data=='silent') $silent=true;
				}
				//для того, чтобы сразу работал рекурсивный вызов блока
				if(!empty($name) && empty($GLOBALS['blocks_data'][$name])) $GLOBALS['blocks_data'][$name]=$e->dynamic->body;
				
				if(!$use_tpl){
					if(empty($blocks[$name])){
						if(!$silent){
							$res2=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
							$res=concat_result($res,$res2,($level==0));
						}
						if(!empty($name)) $GLOBALS['blocks_data'][$name]=$e->dynamic->body;
						if(!empty($GLOBALS["exit"])){ break; /*end_tpl($cache_md5,$res,$lim);  return $res;*/}
						if(!empty($GLOBALS["xbreak"])){ break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}					
					} else {
						$res=concat_result($res,$blocks[$name],($level==0));
					}
				} else {
					if(empty($blocks[$name])){
						$blocks[$name]=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
						if(!empty($name)) $GLOBALS['blocks_data'][$name]=$e->dynamic->body;
						if(!empty($GLOBALS["exit"])){ break;}
						if(!empty($GLOBALS["xbreak"])){ break;}
					}
				}
			}
			if($use_tpl) continue;
			if($e->dynamic->type==TPL_SWITCH){
				$value=parse_var($e->dynamic->ops);
				$wait_default=true;
				if(!empty($e->dynamic->cases)) foreach($e->dynamic->cases AS $case){
					if(seek_equal($value,parse_var($case['case']))){
						$wait_default=false;
						$res2=shell_tpl($case['body'],$cache,$blocks,$level+1);
						$res=concat_result($res,$res2,($level==0));
						break;
					}
				}
				if($wait_default && isset($e->dynamic->default)){
					$res2=shell_tpl($e->dynamic->default,$cache,$blocks,$level+1);
					$res=concat_result($res,$res2,($level==0));
				}
				if(!empty($GLOBALS["exit"])){ break;}
				if(!empty($GLOBALS["xbreak"])){break; }
			}
			if($e->dynamic->type==TPL_SILENT){
				$tmp=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
				if(!empty($GLOBALS["exit"])){ break;}
				if(!empty($GLOBALS["xbreak"])){break;}
			}
			if($e->dynamic->type==TPL_IF){
				$wait_else=true;
				if(do_if($e->dynamic->ops)){
					$wait_else=false;
					$res2=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
					$res=concat_result($res,$res2,($level==0));
				}
				if($wait_else && !empty($e->dynamic->elseif)) foreach($e->dynamic->elseif AS $eif){
					if(do_if($eif['case'])){
						$wait_else=false;
						$res2=shell_tpl($eif['body'],$cache,$blocks,$level+1);
						$res=concat_result($res,$res2,($level==0));
						break;
					}
				}
				if($wait_else && isset($e->dynamic->else)){
					$res2=shell_tpl($e->dynamic->else,$cache,$blocks,$level+1);
					$res=concat_result($res,$res2,($level==0));
				}
				if(!empty($GLOBALS["exit"])){ break;}
				if(!empty($GLOBALS["xbreak"])){break;}
			}
			if($e->dynamic->type==TPL_TREE){
				if($e->dynamic->tree_type==0){
					$GLOBALS["in_tree"]=1;
					$var=parse_var($e->dynamic->ops);
					$rev=0;	
					if(!empty($e->dynamic->tree_rev)) $rev=1;
					if(!empty($e->dynamic->tree_rand)) $rev=2;
					if(!is_numeric($e->dynamic->tree_deep)) $deep=parse_var($e->dynamic->tree_deep);
					else $deep=$e->dynamic->tree_deep;
					$res2=do_tree($e->dynamic->body,$var,$e->dynamic->after,$rev,$deep,0,0,$blocks);
					//if(!empty($GLOBALS["clear"])) $res=$res2; else $res.=$res2;
					$res=concat_result($res,$res2,($level==0));
					if(!empty($GLOBALS["exit"])){ break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}
					if(!empty($GLOBALS["xbreak"])){break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}
				} else if($e->dynamic->tree_type==1) {
					$res2=do_for($e->dynamic->body,parse_var($e->dynamic->tree_start),parse_var($e->dynamic->tree_end),$blocks);
					$res=concat_result($res,$res2,($level==0));
					if(!empty($GLOBALS["exit"])){ break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}
					if(!empty($GLOBALS["xbreak"])){break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}
				}
			}
			if($e->dynamic->type==TPL_WHILE){
				$res2='';
				while(parse_var($e->dynamic->ops)){
					$res2.=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
					if(!empty($GLOBALS["exit"])) break;
					if(!empty($GLOBALS["break"])){$GLOBALS["break"]=false; break; }
					if(!empty($GLOBALS["xbreak"])) break;
				}
				//if(!empty($GLOBALS["clear"])) $res=$res2; else $res.=$res2;
				$res=concat_result($res,$res2,($level==0));
				if(!empty($GLOBALS["exit"])){ break; /*end_tpl($cache_md5,$res,$lim);  return $res;*/}
				if(!empty($GLOBALS["xbreak"])){ break; /*end_tpl($cache_md5,$res,$lim); return $res;*/}
			}
			if($e->dynamic->type==TPL_COMPILE){
				global $tpl_static;
				$type=$e->dynamic->ops[0]->data;
				if(!isset($GLOBALS['compile'])) $GLOBALS['compile']=Array();
				if(!isset($GLOBALS['compile'][$type])) $GLOBALS['compile'][$type]=Array();
				$t=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
				$bool=true;
				$add='';
				if(shift_op($e->dynamic->ops,'u')){
					$add=' u';
					foreach($GLOBALS['compile'][$type] AS $val) if($val==$t){
						$bool=false;
						break;
					}
				}
				if($bool){
					$GLOBALS['compile'][$type][]=$t;
					$tpl_static.='[compile '.$type.$add.'][~'.$t.'~][/compile]';
					//if(strpos($t,'~]')===false) $tpl_static.='[compile '.$type.$add.'][~'.$t.'~][/compile]';
					//else $tpl_static.='[compile '.$type.$add.']'.$e->dynamic->txt_body.'[/compile]';
				}
				if(!empty($GLOBALS["exit"])) break;
				if(!empty($GLOBALS["xbreak"])) break;
			}
			if($e->dynamic->type==TPL_HEAD){
				global $head, $tpl_static;
				$tpl_static.='[head]'.$e->dynamic->txt_body.'[/head]';
				//echo var_dump($e->dynamic->body).'<br><br><br><Br>';
				//echo $GLOBALS["last_op"][count($GLOBALS["last_op"])-1];
				$tmp=shell_tpl($e->dynamic->body,$cache,$blocks,$level+1);
				if(empty($head)) $head=$tmp; else $head.=$tmp;
				if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1) echo $tmp;//на случай если это админка		
			}
			if(!empty($GLOBALS["break"]) || !empty($GLOBALS["continue"])){
				//end_tpl($cache_md5,$res,$lim);
				//return $res;
				break;
			}
		}
	}
	
	if($use_tpl){
		$res=shell_tpl($use_tpl_body,$cache,$blocks,$level+1/* ? */);
	}
	if(!empty($GLOBALS["reset_clear_on_exit"]) && $level<=1){
		$GLOBALS["reset_clear_on_exit"]=false;
		$GLOBALS["clear_once_at_start"]=true;
		//$GLOBALS["clear"]=false;
	}

	if(!empty($GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]) && $GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]==1 && !seek_to_top($GLOBALS["skip_dblspace"],$GLOBALS['lvl2'])){
		if(!is_array($res) && !is_object($res)){
			$res=clear_dblspace($res);
			$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=0;//раскомментил. пока не знаю чем это грозит...
		}
	}
	if(!empty($GLOBALS["skip_space"][$GLOBALS["lvl2"]]) && $GLOBALS["skip_space"][$GLOBALS["lvl2"]]==1 && !seek_to_top($GLOBALS["skip_space"],$GLOBALS['lvl2'])){
		if(!is_array($res) && !is_object($res)){			
			$res=str_replace(' ','',$res);
			$res=str_replace('#bcd1!',' ',$res);
			$GLOBALS["skip_space"][$GLOBALS["lvl2"]]=0;
		}
	}
	if(!empty($GLOBALS["skip_enter"][$GLOBALS["lvl2"]]) && $GLOBALS["skip_enter"][$GLOBALS["lvl2"]]==1 && !seek_to_top($GLOBALS["skip_enter"],$GLOBALS['lvl2'])){
		if(!is_array($res) && !is_object($res)){
			$res=str_replace(chr(10),'',$res); $res=str_replace(chr(13),'',$res);
			$res=str_replace('#bcd2!',chr(10),$res);
			$res=str_replace('#bcd3!',chr(13),$res);
			$GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=0;
		}
	}
	if(!empty($GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]) && $GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]==1 && !seek_to_top($GLOBALS["strip_spaces"],$GLOBALS['lvl2'])){
		if(!is_array($res) && !is_object($res)){
			$res=trim_all($res);
			$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=0;
		}
	}
	if(!empty($GLOBALS["skip_empty"][$GLOBALS["lvl2"]]) && $GLOBALS["skip_empty"][$GLOBALS["lvl2"]]==1 && !seek_to_top($GLOBALS["skip_empty"],$GLOBALS['lvl2'])){
		if(!is_array($res) && !is_object($res)){
			$res=preg_replace("/\s*\r+/", "", $res);
			$GLOBALS["skip_empty"][$GLOBALS["lvl2"]]=0;
		}
	}
	//if(!empty($GLOBALS["clear"])) $GLOBALS["clear"]=false;//под большим вопросом. вообще клеар почему-то нигде не обнуляется после выполнения соответствующей операции.
	end_tpl($cache_md5,$res,$lim,$force_cache);
	$GLOBALS["lvl2"]--;
	return $res;
}

function seek_to_top($array,$index,$value=1){
	for($i=0;$i<$index;$i++){
		if(isset($array[$i]) && $array[$i]==$value) return true;
	}
	return false;
}

// запуск шаблонизатора для административного кабинета

function shell_tpl_admin($tpl_file,&$vars=Array()){
	global $allow_components, $tpl_register_globals, $p_par;
	
	if(file_exists(DOCUMENT_ROOT.'/core/editor/tpl/'.$tpl_file.'.tpl')) $tpl_file=DOCUMENT_ROOT.'/core/editor/tpl/'.$tpl_file.'.tpl';
	else if(file_exists(DOCUMENT_ROOT.'/core/editor/tpl/'.$tpl_file)) $tpl_file=DOCUMENT_ROOT.'/core/editor/tpl/'.$tpl_file;
	else if(!file_exists($tpl_file)) return false;
	
	if(!empty($vars)) foreach($vars AS $var=>$value){
		if(isset($p_par[$var])) $o_var[$var]=$p_par[$var];
		$p_par[$var]=$value;
	}
	
	$old_tpl_register_globals=$tpl_register_globals;
	$old_allow_components=$allow_components;
	
	$tpl_register_globals=true;
	$allow_components['editor']=1;
	
	$result=shell_tpl(file_get_contents($tpl_file));
	
	$tpl_register_globals=$old_tpl_register_globals;
	$allow_components=$old_allow_components;
	
	foreach($vars AS $var=>$value) if(isset($o_var[$var])) $p_par[$var]=$o_var[$var]; else unset($p_par[$var]);
	$vars=Array(); //зануляем vars, p_par пока не трогаем
	
	return $result;
}

?>