<?php

//предназначается для блокировки недопустимых значений в URL
function clear_url($curl){
	$curl=str_replace("%5C","/",$curl);
	$curl=str_replace("\\","/",$curl);
	$curl=str_replace("'","",$curl);
	return $curl;
}

function get_row_url($row){
	if(is_object($row) && isset($row->id)) $row=$row->id;
	if(!is_numeric($row)) return false;
	global $rlink;
	seek_rlink($row);
	if(!isset($rlink[$row])) return false;
	return get_base_url($rlink[$row]->tex,$row);
}

function get_base_url($tex,$owner,$skip_first=0,$step=0){
	global $rlink,$db,$zone;
	global $gbu_module,$gbu_part,$gbu_part2,$gbu_zex,$gbu_zne,$gbu_zones,$gbu_cols, $gbu_zrows,$gbu_ztexs,$gbu_ownr;
	$xn=$owner;
	$wb=Array();
	$tbls=Array();
	$top_tex=$tex;
	if($owner>0){
		seek_rlink_ex($owner);
		$table=$rlink[$owner]->table;
	} else $table=getrowval('SELECT * FROM ex_group WHERE ex_ex1='.$tex,"ex_table");
	//if(!isset($gbu_module[$table])) $gbu_module[$table]=getrowval('SELECT * FROM main_table WHERE table_id='.$table,"table_module");
	//$module=$gbu_module[$table];
	if(!isset($gbu_module[$tex])) $gbu_module[$tex]=getrowval('SELECT * FROM ex_table WHERE ex_id='.$tex,"ex_module");
	$module=$gbu_module[$tex];
	if($owner>0){
		$tbls[]=$table;
		$wb[$table]->tex=$tex;
		$wb[$table]->table=$table;
		$wb[$table]->module=$module;
		$wb[$table]->url=Array();
	}
	seek_rlink_ex($xn);
	while($xn!=0 && !empty($rlink[$xn])){
		if($table!=$rlink[$xn]->table){
			$table=$rlink[$xn]->table;
			$wb[$table]=$rlink[$xn];
			$tbls[]=$rlink[$xn]->table;
			$wb[$table]->url=Array();
		}
		if(!$skip_first) $wb[$table]->url[]=url_fromrow($rlink[$xn]->id,$rlink[$xn]->table);
		$skip_first=false;
		$top_tex=$rlink[$xn]->tex;
		$xn=$rlink[$xn]->owner;
		seek_rlink_ex($xn);
	}
	$url=Array();
	$lpart=0;
	$m=$module;
	//for($i=0;$i<count($tbls);$i++){
	ksort($tbls);
	//if($first_skip && isset($tbls[0])) unset($tbls[0]);
	$tt=0;
	$ui=0;
	$pass_each=0;
	foreach($tbls AS $var=>$tbl)if($pass_each>0) $pass_each--; else if(!empty($tbl)){
		$tt++;
		$i=$var;
		//$tbl=$tbls[$i];
		//echo $tbl.'<br>';
		$w=$wb[$tbl];
		
		if(!isset($gbu_part[$tbl.'.'.$module])) $gbu_part[$tbl.'.'.$module]=getrow($db,"SELECT part_id, part_iowner, part_owner, part_url, part_table, part_ignore FROM main_part WHERE part_ignore=0 AND part_table=$tbl AND part_module=$module");
		$db->Record=$gbu_part[$tbl.'.'.$module];
		if(empty($db->Record)) continue;
		//getrow($db,"SELECT part_id, part_iowner, part_owner, part_url, part_table, part_ignore FROM main_part WHERE part_ignore=0 AND part_table=$tbl");
		
		//echo $db->Record["part_id"].'<br>';
		$zurl=Array();		
		if(!empty($w->url)) foreach($w->url AS $tmp=>$u) $zurl[]=$u;
		krsort($zurl);
		//if(!empty($zurl)) foreach($zurl AS $u){
		//	$url[$ui][]=$u;
		//}
		if(!empty($zurl)){
			$url[$ui]=implode('/',$zurl);
			$ui++;
		}
		if($db->Record["part_url"]!=''){
			$url[$ui]=$db->Record["part_url"];
			$ui++;
		}
		$part_iowner=$db->Record["part_iowner"];
		$stop_skip_table=0;
		while($db->Record["part_owner"]!=0){
			//getrow($db,"SELECT part_id, part_iowner, part_owner, part_url, part_table, part_ignore FROM main_part WHERE part_id=".$db->Record["part_owner"]);
			
			$dbowner=$db->Record["part_owner"];
			if(!isset($gbu_part2[$dbowner])) $gbu_part2[$dbowner]=getrow($db,"SELECT part_id, part_iowner, part_owner, part_url, part_table, part_ignore FROM main_part WHERE part_id=".$dbowner);
			$db->Record=$gbu_part2[$dbowner];
			
			if($db->Record["part_table"]!=0 && $db->Record["part_table"]!=$tbl){
				$stop_skip_table=$db->Record["part_table"];
			} else if($db->Record["part_url"]!=''){
				$url[$ui]=$db->Record["part_url"];
				$ui++;
			}
		}
		$m=$w->module;
		$lpart=$db->Record["part_id"];
		if($part_iowner!=0){
			$xn=0;
			while(!empty($tbls[$i+$xn]) && !empty($wb[$tbls[$i+$xn]]) && $wb[$tbls[$i+$xn]]->module==$m){
				$xn++;
				if ($stop_skip_table!=0 && $tbls[$i+$xn]==$stop_skip_table){
					$xn--;
					break;
				}
			}
			//$i+=$xn;
			if($xn+$tt>=count($tbls)) break;
			//continue($xn);
			if($xn>0) $pass_each=$xn;
			continue;
		}
	}
	krsort($url);
	$last_module=$m;
	$domain='';
	$top_ex=get_ex2($top_tex);
	
	//это по идее должно было отдавать приоритет текущему сайту, но в действительности т.к. top_ex уже определён этого происходить не будет
	/*global $zi;
	foreach($zone AS $var=>$zz) if($zz["zone_id"]==$zi && ($zz["zone_module"]==$m.':'.$top_ex || $zz["zone_module"]==$m.':'.$top_ex.':'.$lpart)){
		$zx=$zone[$var];
		break;
	}*/
	
	if(!isset($zx)) foreach($zone AS $var=>$zz) if(!$zz["zone_redirect"]) if($zz["zone_module"]==$m.':'.$top_ex || $zz["zone_module"]==$m.':'.$top_ex.':'.$lpart){
		$zx=$zone[$var];
		break;
	}
	//foreach($zone AS $zz) if($zz["zone_module"]==$m.':'.$top_ex || $zz["zone_module"]==$m.':'.$top_ex.':'.$lpart){
	if(isset($zx)){
		$domain=$zx["zone_domain"];
		if(empty($domain)){
			$domain=$_SERVER["HTTP_HOST"];
			if(strstr($domain,'/')){
				$domain=explode('/',$domain);
				$domain=$domain[0];
			}
		}
		if(!empty($zx["zone_folder"])) $domain.='/'.$zx["zone_folder"];
		$domain=str_replace('*','',$domain);
		//break;
	}
	if(empty($domain)){
		if(!isset($gbu_zex[$top_tex])) $gbu_zex[$top_tex]=getrowval("SELECT * FROM ex_group WHERE ex_ex1=$top_tex","ex_ex2");
		$zex=$gbu_zex[$top_tex];
		//$zex=getrowval("SELECT * FROM ex_group WHERE ex_ex1=$top_tex","ex_ex2");
		if(!empty($zex)){
			if(!isset($gbu_zne[$zex])) $gbu_zne[$zex]=getrowval("SELECT * FROM ex_zone WHERE ex_module=$zex","ex_zone");
			$zne=$gbu_zne[$zex];
		} else $zne=0;
		//$zne=getrowval("SELECT * FROM ex_zone WHERE ex_module=$zex","ex_zone");
		if(empty($zne) && $GLOBALS['zone'][$GLOBALS['za']]['zone_module']!=-1){
			$zne=$GLOBALS['zone'][$GLOBALS['za']]['zone_id'];
		}
		if(!empty($zne)){
			if(!isset($gbu_zones[$top_tex])) $gbu_zones[$top_tex]=getall4($db,"SELECT * FROM ex_zone WHERE ex_zone=$zne AND ex_module!=$top_tex","ex_module");
			$exs=$gbu_zones[$top_tex];
			//$exs=getall4($db,"SELECT * FROM ex_zone WHERE ex_zone=$zne AND ex_module!=$top_tex","ex_module");
			if(!empty($exs)){
				$zmdls=Array();
				$zexs=Array();
				$ztexs=Array();
				foreach($exs AS $var=>$value){
					$zmdls[$value["ex_module2"]]=$value["ex_module2"];
					$zexs[$value["ex_module"]]=$value["ex_module"];
				}
				$cimp=implode(',',$zmdls);
				if(!isset($gbu_cols[$cimp])) $gbu_cols[$cimp]=getall4($db,"SELECT col_module,col_type,col_id FROM main_col WHERE col_module IN (".$cimp.") AND col_type=4","col_id");
				$cols=$gbu_cols[$cimp];
				//$cols=getall4($db,"SELECT col_module,col_type,col_id FROM main_col WHERE col_module IN (".implode(',',$zmdls).") AND col_type=4","col_id");
				$nmdls=Array();
				foreach($cols AS $col)$nmdls[$col["col_module"]]=$col["col_module"];
				$cols=array_keys($cols);
				if(!empty($zexs) && !empty($nmdls)){
					$bz=implode(',',$zexs).'.'.implode(',',$nmdls);
					if(!isset($gbu_ztexs[$bz])) $gbu_ztexs[$bz]=getall3($db,"SELECT * FROM ex_group WHERE ex_ex2 IN (".implode(',',$zexs).") AND ex_module IN (".implode(',',$nmdls).")","ex_ex1");
					$ztexs=$gbu_ztexs[$bz];
					//$ztexs=getall3($db,"SELECT * FROM ex_group WHERE ex_ex2 IN (".implode(',',$zexs).") AND ex_module IN (".implode(',',$nmdls).")","ex_ex1");
				}
				if(!empty($ztexs)){
					$bz=implode(',',$ztexs);
					if(!isset($gbu_zrows[$bz])) $gbu_zrows[$bz]=getall3($db,"SELECT * FROM row_owner WHERE ro_ex IN (".$bz.")","row_id");
					$zrows=$gbu_zrows[$bz];
					//$zrows=getall3($db,"SELECT * FROM row_owner WHERE ro_ex IN (".implode(',',$ztexs).")","row_id");					
				}
				if(!empty($zrows) && !empty($cols)){
					$m1=$last_module.':'.$top_ex;
					$m2=$last_module.':'.$top_ex.':'.$lpart;
					$bz=implode(',',$zrows).'.'.implode(',',$cols).'.'.$m1.'.'.$m2;
					if(!isset($gbu_ownr[$bz])) $gbu_ownr[$bz]=getrow($db,"SELECT * FROM row_value WHERE value_row IN (".implode(',',$zrows).") AND value_col IN (".implode(',',$cols).") AND (value_value='$m1' OR value_value='$m2')");
					$db->Record=$gbu_ownr[$bz];
					//getrow($db,"SELECT * FROM row_value WHERE value_row IN (".implode(',',$zrows).") AND value_col IN (".implode(',',$cols).") AND (value_value='$m1' OR value_value='$m2')");
					$ownr=$db->Record["value_row"];
					$ownert=$db->Record["value_table"];
					if(!empty($ownr)){
						seek_rlink_ex($ownr);
						$domain=get_base_url($rlink[$ownr]->tex,$rlink[$ownr]->id,$skip_first,$step+1);
					}
				}
			}
		}
	}
	if(!empty($domain)){
		//rsort($url);
		if(!empty($GLOBALS["base_root"]) && $step==0) $pref=$GLOBALS['base_root']; else $pref='';
		$x=$domain.$pref.'/'.implode('/',$url);
		while(strstr($x,'http://')) $x=str_replace('http://','',$x);	
		while(strstr($x,'//')) $x=str_replace('//','/',$x);
		return $GLOBALS['prot'].'://'.$x;
	} else return '';
}

function get_admin_url($row=0,$table_id=0,$table_ex=0,$table_owner=0){
	if(!empty($row)){
		global $rlink;
		seek_rlink($row);
		$trow=get_top_row($rlink[$row]->id);
		if(!is_object($trow)) $trow=$rlink[$row];
		return 'mod_table?id='.$trow->module.'&amp;id2='.$trow->tex.'&amp;id3='.$rlink[$row]->id.'&amp;id4=0&amp;id5='.$rlink[$row]->rid.'&amp;id6='.$rlink[$row]->owner.'&amp;id7='.$rlink[$row]->table.'&amp;action=edit_form#editform';
	} else {
		$module_id=getrowval("SELECT table_module FROM main_table WHERE table_id=$table_id","table_module");
		return 'mod_table?id='.$module_id.'&amp;id2='.$table_ex.'&amp;id6='.$table_owner.'&amp;id7='.$table_id;
	}
}

function url_col($table){
	global $db,$umj;
	if(!isset($umj[$table])){
		getrow($db,"SELECT * FROM main_col WHERE col_table=$table AND col_url=1",1,'main_col',__LINE__,__FILE__);
		if(empty($db->Record)){
			$umj[$table]=0;
			return 0;
		}
		$umj[$table]=$db->Record;
		return $umj[$table];
	} else return $umj[$table];
	return 0;
}

function url_fromrow($row,$table){
	global $prow,$db,$tree_vars;
	$col=url_col($table);
	$col3=$col;
	$col2=$col["col_id"];
	$col=$col["col_sname"];
	if(empty($col)) return '';//такое было на иноках... жесть
	if(isset($GLOBALS["spec_step2"])) $tree_vars[$GLOBALS["spec_step2"]][$col2]=$col3;

	if(!isset($prow[$row][$col])){
		prepend_row($row);

		//закомментить участок ниже для достяжения ускорения
		global $vcache2;
		if(isset($vcache2[$col2][$row])){
			$prow[$row][$col]=$vcache2[$col2][$row]["value_value"];
			return $vcache2[$col2][$row]["value_value"];
		} else {
			//был перемещён сюда, чтобы не захватывала кешированные параметры
			//if(isset($GLOBALS["spec_step2"])) $tree_vars[$GLOBALS["spec_step2"]][$col2]=$col3;
			//и был убран отсюда, т.к. проверку логичней делать внутри tree_optimize
		}
		getrow($db,"SELECT * FROM row_value WHERE value_col=$col2 AND value_row=$row",1,"row_value",__LINE__,__FILE__);
		$vcache2[$col2][$row]=$db->Record;
		if(!empty($db->Record)){
			if(!empty($col3["col_force_onshow"])) $db->Record["value_value"]=force_value($db->Record,$col3);
			$prow[$row][$col]=$db->Record["value_value"];
			return $db->Record["value_value"];
		}
		//закомментить участок выше. и расскомментить prepend_row
		//а нужно это для првиедение всех запросов к виду SELECT * FROM row_value WHERE value_col=X AND value_row=Y
		//что необходимо для дальнейшей оптимизации
	}
	if(!empty($prow[$row][$col])) return $prow[$row][$col]; else return '';
}

function convert_url($host,$amp=0,$param=Array()){	//если amp=0 , то используется & иначе &amp;
											//хост принимается вида http://site.ru (без слеша на конце)
	global $e5uid;
	if($amp==0) $am='&'; else $am='&amp;';
	if(strstr($host,'?')){
		$host=explode('?',$host);
		$ap=explode($am,$host[1]);
		$host=$host[0];
		foreach($ap AS $aps){
			$aps=explode('=',$aps);
			if(!isset($param[$aps[0]]) && isset($aps[1])) $param[$aps[0]]=$aps[1];
		}
	}
	$url=$host;
	if($host[strlen5($host)-1]=='/') $host=substr5($host,0,strlen($host)-1);
	$pstr='';$pstr2='';
	$hq=0;
	foreach($param AS $var=>$p){
		if(!empty($pstr)) $pstr.=$am; else $pstr='?';
		$pstr.=$var.'=';
		$pstr.=urlencode('^^'.serialize($p));

		if(!empty($pstr2)) $pstr2.=$am; else $pstr2='?';
		$pstr2.=$var.'=';
		$pstr2.='^^'.serialize($p);
	}
	if($pstr=='') return $url;
	$safe2=md5($pstr2./*date('d').md5($url).*/md5($e5uid));
	//md5(url) и ниже md5(host) были упразднены из-за различия в трактовке кирилических доменов
	$safe='';for($i=0;$i<strlen($safe2);$i++)if($i%7==0) $safe.=$safe2[$i];
	$pstr.=$am.'safe='.$safe;
	return $url.$pstr;
}

function check_url($url){
	global $e5uid,$db;
	//$host=explode('/',$url);
	//$host=$host[0].'//'.$host[2];
	if(strstr($url,'?')){$host=explode('?',$url); $host=$host[0];}
	else return true;
	if($host[strlen5($host)-1]=='/') $host=substr5($host,0,strlen($host)-1);
	$pstr=explode('?',$url);
	$pstr=$pstr[1];
	$am='&';if(strstr($pstr,'&amp;')) $am='&amp;';
	$pstr2=explode($am.'safe=',$pstr);
	$have_postfix=strpos($pstr2[1],'&');
	$pstr2=urldecode($pstr2[0]);
	$param=Array();
	if(strstr($pstr,$am)) $ps=explode($am,$pstr);
	else {$ps=Array(); $ps[0]=$pstr;}
	$safe='';
	foreach($ps AS $p){
		$p=explode('=',$p);
		if($p[0]!='safe'){
			if(isset($param[$p[0]])){
				$_GET[$p[0]]=$param[$p[0]];
				continue;
			}
			$param[$p[0]]=urldecode($p[1]);
			if(strlen($param[$p[0]])>=2 && $param[$p[0]][0]=='^' && $param[$p[0]][1]=='^') $param[$p[0]]=string_to_array(substr($param[$p[0]],2));
			else {
				$_GET[$p[0]]=$param[$p[0]];
			}
		}
		else $safe=$p[1];
	}	
	$safe2B=md5('?'.$pstr2.date('d')./*md5($host).*/md5($e5uid));
	$safeB='';for($i=0;$i<strlen($safe2B);$i++)if($i%7==0) $safeB.=$safe2B[$i];

	//это чтобы при смещении суток Ajax скрипты не вылетали
	$d=day_plus(date('Y-m-d'),-1);$d=explode('-',$d);$d=$d[2];
	$safe2C=md5('?'.$pstr2./*$d.md5($host).*/md5($e5uid));
	$safeC='';for($i=0;$i<strlen($safe2C);$i++)if($i%7==0) $safeC.=$safe2C[$i];

	$skiptest=0;
	$param["part"]=0;
	if(!empty($param["ajax"])){
		$var=explode('.',$param["ajax"]);
		if($var[0]=='form')$type=3;
		if($var[0]=='component')$type=2;
		if($var[0]=='show')$type=1;
		if($var[0]=='function' || $var[0]=='func')$type=0;	
	}
	if(!isset($type)){
		getrow($db,"SELECT * FROM main_module WHERE module_sname='$var[0]'",1,"main_module");
		if(!empty($db->Record)) getrow($db,"SELECT * FROM main_part WHERE part_module=".$db->Record["module_id"]." AND part_sname='$var[1]'",1,"main_part");
		if(!empty($db->Record)){
			$param["part"]=$db->Record["part_id"];
			if($db->Record["part_unsafe"]) $skiptest=1;
		}
	} else {
		getrow($db,"SELECT * FROM main_part WHERE part_type=2 AND part_proc=$type AND part_sname='$var[1]'",1,"main_part");
		if(!empty($db->Record)){
			$param["part"]=$db->Record["part_id"];
			if($db->Record["part_unsafe"]) $skiptest=1;
		}
	}
	//if(empty($safe) && !$skiptest) return false;

	//echo $safeB.' - '.$safe.' - '.$safeC;

	//return $param;
	if($skiptest) return $param;
	
	if($safeB!=$safe && $safeC!=$safe/* && $skiptest==0*/) return false;
	
	//if($have_postfix && $skiptest==0) return false;
	
	return $param;
}

function cut_left_url($url,$count){
	global $left_url, $right_url, $min_url;
	$left_url=$url;
	$right_url=Array();
	for($i=0;$i<$count;$i++){
		if(isset($left_url[0])) $right_url[]=$left_url[0];
		$left_url=del1($left_url);
	}
	$t=implode('/',$left_url);
	if(!isset($min_url) || strlen($t)<strlen($min_url)) $min_url=$t;
	if(empty($left_url)) $left_url=Array('');
	if(empty($right_url))$right_url=Array('');
}

//convert_url('http://www.test1.ru',0,Array("x"=>2,"y"=>"adada2"));
//check_url('http://www.test1.ru?x=2&y=adada2&safe=f4f2a');

function select_part($part,$mdata,$url,$module_id,$module_ex,$level=0,$url_step=0,$owner_row=0,$table_id=0,$is_404=0){	
	global $case_url,$rlink,$last_cow;
	$b_case=$case_url;
	if(!empty($url[$url_step])) $case_url=$url[$url_step]; else $case_url='';
	if(!empty($part["part_ifcase"]) && !prep_do_if($part["part_ifcase"],$part,$owner_row,$module_ex,$url)) return false;
		
	$result->row=$owner_row;
	$result->part=$part['part_id'];
	$result->module_ex=$module_ex;
	$result->module_id=$module_id;
	$result->table_id=$table_id;
	$result->is_404=$is_404;
	$result->level=$level;
	$result->url_step=$url_step;
	if(!empty($result->row)){
		$result->data['value_table']=$result->table_id;
		$result->data['value_module']=$result->module_id;
		$result->data['value_row']=$result->row;
	} else {
		$result->data=$result->part;
	}
	if(!empty($part["part_ifrow"])){
		unset($result->data);
		$tmp=prep_parse_var($part["part_ifrow"],$module_id,$module_ex,$result->part,$owner_row,$url);
		if(!is_object($tmp)){
			seek_rlink($tmp);
			$tmp=$rlink[$tmp];
		}
		$result->data["value_table"]=$tmp->table;
		$result->data["value_module"]=$tmp->module;
		$result->data["value_row"]=$tmp->id;
		$result->row=$tmp->id;
		if(strstr($part['part_ifrow'],'case_url')) $url_step++;//$result->skip_url;
	}
	$GLOBALS["mod_part"][$module_id]=$part['part_id'];
	$GLOBALS["mexs"][$result->module_id]=$module_ex;
	$GLOBALS["parts_cow"][$result->part]=$result->row;
	if(!empty($result->row)) $last_cow=$result->row;
	if(!empty($result->row)) $GLOBALS["selected_rows"][$result->row]=$result->part;
	
	if(!empty($result->row) && !empty($result->data['value_table'])) $GLOBALS["url_row"][$result->data['value_table']]=$result->row;;
	if(!empty($result->row) && !empty($result->data['value_module'])) $GLOBALS["url_row2"][$result->data["value_module"]]=$result->row;
	if(!empty($result->module_ex) && !empty($result->data['value_table'])) $GLOBALS["url_row3"][$result->module_ex]=$result->data["value_table"];	
	
	cut_left_url($url,$url_step);//под вопросом
	
	if(!empty($part['part_skipurl'])){
		$GLOBALS['left_url_b']=$GLOBALS['left_url'];
		cut_left_url($GLOBALS['left_url'],$part['part_skipurl']);
	} else $GLOBALS['left_url_b']='';
	
	part_history($result,$module_ex,$module_id);
	$case_url=$b_case;
	if(empty($url[$url_step])) $ignore_404=true;
	else $ignore_404=false;
	$nested_r=prepend_url($mdata, $url, $part['part_id'], $owner_row, $level+1, $url_step);
	if($nested_r!=false && (!$ignore_404 || !$nested_r->is_404)) return $nested_r;
	else return $result;
}

function prepend_url($mdata, $url, $owner=0, $owner_row=0, $level=0, $url_step=0, $check_404=0){
	//mdata - module_id:module_ex:[module_part]
	//url - array of url-s for parsing
	//owner - part-owner
	//level - nesting depth
	global $db,$left_url,$right_url,$rlink/*,$next_url_step*/,$is_url_parse;
	if(empty($is_url_parse)) $is_url_parse=1; else $is_url_parse++;
	/*if($next_url_step){
		$url_step+=$next_url_step;
		//cut_left_url($url,$next_url_step);
		$old_next_url_step=$next_url_step;
		$next_url_step=0;
	}*/
	if(strstr($mdata,'?')){
		$mdata=explode('?',$mdata);
		$owner_row=$mdata[1];
		$mdata=$mdata[0];
	}
	$tmp=explode(':',$mdata);
	$module_id=$tmp[0];
	if(empty($module_id)){
		$is_url_parse--;
		return false;
	}
	if(isset($tmp[1])) $module_ex=$tmp[1]; else $module_ex=0; //ситуация, когда module_ex==0 не совсем ясна...
	if(!empty($tmp[2]) && empty($owner)){
		// работа при известной текущей части
		$manual_setter=1;
		$SQL_part_where='part_id='.$tmp[2];
		$SQL_part_ignore='';
	} else {
		// работа без известной первоначальной части
		$manual_setter=0;
		$SQL_part_where='part_owner='.$owner;
		$SQL_part_ignore='part_ignore=0 AND ';
		if($owner==0) $SQL_part_where.=' AND part_module='.$module_id;
	}
	if($check_404) $SQL_part_where.=' AND part_404=1';
	$parts=getall($db,'SELECT part_id, part_table, part_url, part_iowner, part_sowner, part_404, part_ifcase, part_ifdetect, part_ifrow, part_module, part_skipurl FROM main_part WHERE '.$SQL_part_ignore.$SQL_part_where.' AND part_enable=1');
	if(empty($parts)){
		$is_url_parse--;
		return false;
	}
	
	// Поиск по точному совпадению в URL части
	foreach($parts AS $part) if($part['part_table']==0 && empty($part['part_ifdetect']) && !empty($url[$url_step]) && $part['part_url']==$url[$url_step]){
		$r=select_part($part,$mdata,$url,$module_id,$module_ex,$level,$url_step+1,$owner_row,0,0);
		if($r){
			$is_url_parse--;
			return $r;
		}
	}
	
	// Поиск по связной таблице (первый проход - без пустых URL, второй - пустые URL)
	for($can_empty=0;$can_empty<2;$can_empty++) foreach($parts AS $part) if($part['part_table']!=0){
		$u_step=$url_step;
		if($part['part_url']!=''){
			if(empty($url[$url_step])) $url[$url_step]='';
			if($part['part_url']==$url[$url_step]){
				$u_step++;
			} else if(!$manual_setter) continue;
		}

		$table=$part['part_table'];
		$row_id=0;
		// проверка валидности owner_row
		$b_owner_row=$owner_row;
		if(!empty($b_owner_row)){
			seek_rlink($b_owner_row);
			$tmp=get_all_sub_tables(Array(),Array($rlink[$b_owner_row]->table));
			//var_dump($tmp); echo '<br><br>'.$rlink[$owner_row]->table.' = '.$table;
			if(empty($tmp[$table])) $b_owner_row=0;
		}
		if($part['part_iowner'] && !empty($url[$u_step]) && !$can_empty){
			// iowner
			// этот метод не будет работать с клонированными подэлементами из других экземпляров
			// для подобной работы используйте собственный метод в part_ifdetect						
			if($b_owner_row==0){
				$texes=get_all_table_ex($table,$module_id,$module_ex);
			} else {
				$texes=Array($rlink[$b_owner_row]->tex);
			}
			$v=safe_sql_input($url[$u_step]);
			$SQL_url_where=' AND r.row_table='.$table;
			/*if(!empty($part['part_sowner'])){
				$SQL_url_where='';
			}  else {
				$SQL_url_where=' AND r.ro_sub='.$b_owner_row;
			}*/
			$rows=getall($db,"SELECT r.row_id, r.row_table, r.ro_user, r.ro_users FROM row_owner AS r LEFT JOIN row_value AS v ON v.value_row=r.row_id LEFT JOIN main_col AS c ON c.col_id=v.value_col WHERE r.ro_ex IN (".implode(',',$texes).") AND r.ro_enable=1".$SQL_url_where." AND c.col_url=1 AND v.value_value='$v'");			
			if(!empty($rows)) foreach($rows AS $row) if(empty($b_owner_row) || have_owner($row['row_id'],$b_owner_row)){
				//wayback check auth perms
				if(check_row($row['row_id'],$row['row_table'],$module_ex,'view',$row['ro_user'],$row['ro_users'],$module_id)){
					$row_id=$row['row_id'];
					$row_table=$row['row_table'];
					$u_step++;
					break;
				}
			}
		} else {
			// standart
			if($b_owner_row==0){
				$tex=get_tex(0,$module_ex,$part['part_table']);
				if(empty($tex)){
					$is_url_parse--;
					return false;
				}
			} else {
				seek_rlink($b_owner_row);
				$tex=$rlink[$b_owner_row]->tex;
			}
			$r_owner=$b_owner_row;
			$r_level=0;
			$SQL_url_where='';
			if(empty($part['part_sowner'])) $SQL_url_where=' AND r.ro_sub='.$b_owner_row;
			do{
				$r_continue=false;
				if(empty($url[$u_step])){
					if($r_level==0) $v=''; else break;
				} else $v=safe_sql_input($url[$u_step]);
				$SQL_url_table='';
				if($r_level==0) $SQL_url_table=' AND r.row_table='.$table;				
				$SQL_value="v.value_value='$v'";
				if($r_level!=0) $SQL_value="(v.value_value='$v' OR v.value_value='')";
				if($r_level==0 && $can_empty) $SQL_value="v.value_value=''";
				$rows=getall($db,"SELECT r.row_id, r.row_table, r.ro_user, r.ro_users, v.value_value FROM row_owner AS r LEFT JOIN row_value AS v ON v.value_row=r.row_id LEFT JOIN main_col AS c ON c.col_id=v.value_col WHERE r.ro_ex=$tex".$SQL_url_table." AND r.owner_id=$r_owner".$SQL_url_where." AND r.ro_enable=1 AND c.col_url=1 AND ".$SQL_value." ORDER BY v.value_value DESC");
				foreach($rows AS $row){
					if(check_row($row['row_id'],$row['row_table'],$module_ex,'view',$row['ro_user'],$row['ro_users'],$module_id)){
						$row_id=$row['row_id'];
						$row_table=$row['row_table'];
						$r_owner=$row_id;
						$r_level++;
						if($row['value_value']!='') $u_step++;
						$r_check_404=empty($row['value_value']);
						$r_continue=true;
						break;
					}
				}
				if(!$r_continue) break;
			} while (true);
		}
		if($row_id!=0){
			$r=select_part($part,$mdata,$url,$module_id,$module_ex,$level,$u_step,$row_id,$row_table,0);
			if($r){
				$is_url_parse--;
				if(!empty($r_check_404)) $r->check_404=true;
				return $r;
			}
		}
	}
	
	// Поиск части по условию ifdetect
	foreach($parts AS $part) if($part['part_table']==0 && !empty($part['part_ifdetect'])){
		$u_step=$url_step;
		if($part['part_url']!=''){
			if($part['part_url']==$url[$url_step]){
				$u_step++;
			} else continue;
		}
		if(prep_do_if($part['part_ifdetect'],$part,0,$module_ex,$url,$u_step,1)){
			if(strstr($part['part_ifdetect'],'case_url') && !strstr($part['part_ifrow'],'case_url')) $u_step++;
			$r=select_part($part,$mdata,$url,$module_id,$module_ex,$level,$u_step,$owner_row,0,0);
			if($r){
				$is_url_parse--;
				return $r;
			}
		}
	}
	
	// Поиск части с пустым URL
	foreach($parts AS $part) if(($part['part_url']=='' && !$part['part_404'] && empty($part['part_ifdetect']) &&  $part['part_table']==0) || $manual_setter){
		$r=select_part($part,$mdata,$url,$module_id,$module_ex,$level,$url_step,$owner_row,0,0);
		if($r){
			$is_url_parse--;
			$r->check_404=true;
			return $r;
		}
	}
	
	// Поиск части исключения (если не найдена ни одна часть)
	foreach($parts AS $part) if($part['part_404']){
		$r=select_part($part,$mdata,$url,$module_id,$module_ex,$level,$url_step,$owner_row,0,1);
		if($r){
			$is_url_parse--;
			$GLOBALS['min_url']='';
			return $r;
		}
	}
	
	$is_url_parse--;
	return false;
}

?>