<?php

function shell_part_name($part_type='function',$part_sname=''){
	global $db;
	if($part_type=='function' || $part_type=='func') $part_proc=0;
	if($part_type=='show') $part_proc=1;
	if($part_type=='comp' || $part_type=='component') $part_proc=2;
	if($part_type=='form') $part_proc=3;
	getrow($db,"SELECT * FROM main_part WHERE part_sname='$part_sname' AND part_proc=$part_proc",1,"main_part");
	if(empty($db->Record["part_id"])) return '';
	else return shell_part($db->Record["part_id"]);
}

function start_part_shell($part_id){
	global $do_part_log;
	if(empty($part_id)) return false;
	if(!$do_part_log) return false;
	global $part_shell_log;
	if(!isset($part_shell_log)) $part_shell_log=Array();
	$part_shell_log[count($part_shell_log)]=microtime(1);
}

function end_part_shell($part_id){
	global $do_part_log;
	if(empty($part_id)) return false;
	if(!$do_part_log) return false;
	global $part_shell_log,$db;
	$cl=count($part_shell_log)-1;
	$time=microtime(1)-$part_shell_log[$cl];
	if(isset($part_shell_log[$cl-1])) $part_shell_log[$cl-1]+=$time;
	unset($part_shell_log[$cl]);
	$time=substr($time,0,5);
	getrow($db,"SELECT part_shellcount, part_shelltime FROM main_part WHERE part_id=$part_id");
	$total_time=$db->Record["part_shelltime"]+$time;
	$total_count=$db->Record["part_shellcount"]+1;
	//$db->query("UPDATE main_part SET part_shellcount=part_shellcount+1, part_shelltime=part_shelltime+".$time." WHERE part_id=".$part_id);
	$db->query("UPDATE main_part SET part_shellcount=$total_count, part_shelltime=$total_time WHERE part_id=".$part_id);
}

function shell_part($part,$ex=0,$row=0){
	stf('shell_part');
	global $db,$partcat,$user;
	$GLOBALS["lvl"]++;
	if(empty($GLOBALS["lvl2"])) $GLOBALS["lvl2"]=1;
	$result='';
	global $part_cache4;
	if(!isset($part_cache4[$part])){
		getrow($db,"SELECT * FROM main_part WHERE part_id=$part",1,'main_part',__LINE__,__FILE__);
		$part_cache4[$part]=$db->Record;
	}
	$db->Record=$part_cache4[$part];
	if($db->Record['part_enable']==0){
		etf('shell_part');
		$GLOBALS["lvl"]--;
		return $result;
	}
	$dbr=$db->Record;
	$force_auth=false;
	if((isset($dbr["part_auth"]) && $dbr["part_auth"]==0 && !$user->super) || (isset($dbr["part_auth"]) && $dbr["part_auth"]>0 && $dbr["part_auth"]!=$user->id)){
		if($dbr["part_auth"]==0) $force_auth='super'; else $force_auth=$dbr["part_auth"];
	}
	$res=backup_globals($force_auth);
	$GLOBALS["spec_step"]=0;unset($GLOBALS["up2"]);unset($GLOBALS["up"]);//experemental
	$GLOBALS["cur_part"]=$part;

	if(empty($ex) && $dbr["part_ex"]>0) $GLOBALS["cur_ex"]=$dbr["part_ex"]; else $GLOBALS["cur_ex"]=$ex;
	if(isset($dbr["part_type"]) && $dbr["part_type"]!=2 && !empty($dbr["part_module"]))$GLOBALS["cur_module"]=$dbr["part_module"];
	if(!empty($row)) $GLOBALS["cur_row"]=$row;
	// здесь должно быть присоединение к part_body cat_top и cat_bottom если part имеет cat
	if(!empty($dbr)){
		do_log("Shell part ".$dbr["part_name"].'/'.$dbr["part_sname"]);
		$prt=$dbr;
		$after='';$pre='';
		if($prt["part_cat"]!=0){
			if(empty($partcat[$prt["part_cat"]])){
				getrow($db,"SELECT * FROM part_cat WHERE cat_id=".$prt["part_cat"],1,"part_cat",__LINE__,__FILE__);
				$partcat[$prt["part_cat"]]=$db->Record;
			}
			$after=$partcat[$prt["part_cat"]]["cat_after"];
			$pre=$partcat[$prt["part_cat"]]["cat_pre"];
		}
		$result='';
		if(empty($GLOBALS["part_stop"])) $GLOBALS["part_stop"]='';
		$old_part_stop=$GLOBALS["part_stop"];

		if(!empty($dbr["part_cur"])){
			$part_cur=explode(':',$dbr["part_cur"]);
			$GLOBALS["cur_row"]=$part_cur[2];
			$GLOBALS["cur_table"]=$part_cur[1];
			$GLOBALS["cur_ex"]=$part_cur[0];
			$db->Record=table_cache($part_cur[1]);
			$GLOBALS["cur_id"]=$GLOBALS["cur_table"].':'.$GLOBALS["cur_ex"].':'.$GLOBALS["cur_row"].':'.$db->Record["table_sname"].':'.get_tex($GLOBALS["cur_row"],$GLOBALS["cur_ex"],$GLOBALS["cur_table"]);
		}
		if(!empty($prt["part_ifrow"])){
			if(isset($GLOBALS["parts_cow"][$prt["part_id"]])){
				$GLOBALS["cur_row"]=$GLOBALS["parts_cow"][$prt["part_id"]];
			} else {
				$GLOBALS["cur_row"]=get_row_id(parse_var($prt["part_ifrow"]));
				$GLOBALS["parts_cow"][$prt["part_id"]]=$GLOBALS["cur_row"];
			}
		}

		if(!empty($pre)) $result2=shell_tpl($pre); else $result2='';
		$result=concat_result($result,$result2,1);
		if(!empty($GLOBALS["exit"])){ return_globals($res); etf('shell_part'); $GLOBALS["lvl"]--; return $result;}

		if(empty($GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]])) $GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=0;
		$old_skip=$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]];
		//$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=0;
	
		if(empty($GLOBALS["strip_spaces"][$GLOBALS["lvl2"]])) $GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=0;
		$old_strip=$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]];
		//$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=0;

		if(empty($GLOBALS["skip_space"][$GLOBALS["lvl2"]])) $GLOBALS["skip_space"][$GLOBALS["lvl2"]]=0;
		$old_skip2=$GLOBALS["skip_space"][$GLOBALS["lvl2"]];
		//$GLOBALS["skip_space"][$GLOBALS["lvl2"]]=0;
		
		if(empty($GLOBALS["skip_enter"][$GLOBALS["lvl2"]])) $GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=0;
		$old_skip3=$GLOBALS["skip_enter"][$GLOBALS["lvl2"]];
		
		if(empty($GLOBALS["skip_empty"][$GLOBALS["lvl2"]])) $GLOBALS["skip_empty"][$GLOBALS["lvl2"]]=0;
		$old_skip4=$GLOBALS["skip_empty"][$GLOBALS["lvl2"]];
		//$GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=0;
		
		start_part_shell($prt["part_id"]);
		if($prt["part_parse"]==0){
			$cache_conf=get_cache_conf();
			global $tpl_static; $ts_back=$tpl_static; $tpl_static='';
			
			$butpl=start_part_c($part);
			global $puid; $bpuid=$puid; global $p_par; $puid=/*get_uid($GLOBALS["p_par"])*/md5(serialize($p_par));	
				//
				$result2=shell_tpl($prt["part_body"]);	// ! вот он запуск части
				//
			$puid=$bpuid;
			end_part_c($butpl);
			
			$tpl_static=$ts_back.$tpl_static;
			set_cache_conf($cache_conf);
		} else if($prt["part_parse"]==1){
			$result2=shell_php($prt["part_body"]);
		}
		end_part_shell($prt["part_id"]);
		$GLOBALS["xbreak"]='';

		$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=$old_skip;
		$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=$old_strip;
		$GLOBALS["skip_space"][$GLOBALS["lvl2"]]=$old_skip2;
		$GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=$old_skip3;
		$GLOBALS["skip_empty"][$GLOBALS["lvl2"]]=$old_skip4;

		$result=concat_result($result,$result2,1);
		
		if($GLOBALS["part_stop"]==1  && $old_part_stop==0){ $GLOBALS["clear"]=1; $GLOBALS["exit"]=1; } // перенёс сюда с низу
	
		if(!empty($GLOBALS["exit"])){etf('shell_part'); return_globals($res); $GLOBALS["lvl"]--; return $result;}


		// AFTER BLOCK

		if(!empty($after)){
			$result2=shell_tpl($after);
			$result=concat_result($result,$result2,1);
		}
		if(!empty($GLOBALS["exit"])){return_globals($res); etf('shell_part'); $GLOBALS["lvl"]--;  return $result;}
		
		//if($GLOBALS["part_stop"]==1  && $old_part_stop==0){ $GLOBALS["clear"]=1; $GLOBALS["exit"]=1; }

	}
	return_globals($res);
	etf('shell_part');
	$GLOBALS["lvl"]--;
	return $result;
}

function check_404_page($res,$module,$cpart=0,$skip_shell=0,$op=Array()){
	if(isset($res->check_404) && $res->check_404 && !empty($GLOBALS['min_url'])){
		$u=shell_module($module,$cpart,$skip_shell,$op,1);
		if(!($u===false)) return Array('r'=>true,'data'=>$u);
	}
	return false;
}

function shell_php($body){
	$temp=ob_get_contents();
	if($temp) ob_end_clean();
	ob_start();
	eval_e5($body);
	$tmp2=ob_get_contents();
	ob_end_clean();
	if($temp){
		ob_start();
		echo $temp;
	}
	$result2=$tmp2;
	if($GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]==1){
		if(!is_array($result2) && !is_object($result2)) $result2=clear_dblspace($result2);
	}
	if($GLOBALS["skip_space"][$GLOBALS["lvl2"]]==1){
		if(!is_array($result2) && !is_object($result2)){
			$result2=str_replace(' ','',$result2);
			$result2=str_replace('#bcd1!',' ',$res);
		}
	}
	if($GLOBALS["skip_enter"][$GLOBALS["lvl2"]]==1){
		if(!is_array($result2) && !is_object($result2)){
				$result2=str_replace(chr(10),'',$result2); $result2=str_replace(chr(13),'',$result2);
				$result2=str_replace('#bcd2!',chr(10),$result2);
				$result2=str_replace('#bcd3!',chr(13),$result2);
		}
	}
	if($GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]==1){
		if(!is_array($result2) && !is_object($result2)) $result2=trim_all($result2);
	}
	if($GLOBALS["skip_empty"][$GLOBALS["lvl2"]]==1){
		if(!is_array($result2) && !is_object($result2)) $result2=preg_replace("/\s*\r+/", "", $result2);
	}
	
	return $result2;
}

function shell_module($module,$cpart='',/*$var=Array()*/$skip_shell=0,$op=0,$check_404=0){//  /*var - для дальнейшего разбора модуля*/ /*op - для вытаскиваний из put параметров, передаваемых вызываемой части*/
	global $left_url,$right_url,$db,$roun,$pmres;
	$result='';	
	$GLOBALS["lvl"]++;
	if(empty($GLOBALS["lvl2"])) $GLOBALS["lvl2"]=1;
	if(empty($roun)) $roun2=''; else $roun2=$roun;
	if(empty($roun)) $roun=''; else if($roun[strlen($roun)-1]!='/') $roun.='/';
	if(empty($right_url)) $right_url=Array('');
	$roun=$roun.implode('/',$right_url);
	$module2=$module;
	if(strstr($module,':')){
		$xm=explode(':',$module);
		$module=$xm[0];
		if(strstr($xm[1],'?')){
			$master_owner=explode('?',$xm[1]);
			$master_owner=$master_owner[1];
		}
		if(isset($xm[1])) $module.=':'.$xm[1];
	}
	if(empty($pmres[$module]) || $check_404){
		$res=prepend_url($module2,$left_url,0,0,0,0,$check_404);
		if(!$res && $check_404) return false;
		$pmres[$module]->res=$res;
		$pmres[$module]->left_url=$GLOBALS["left_url"];
		$pmres[$module]->right_url=$GLOBALS["right_url"];
		$pmres[$module]->roun=$roun;
		if(!empty($GLOBALS['pup'])) $pmres[$module]->pup=$GLOBALS['pup'];
		else $pmres[$module]->pup=Array();
	}
	else {
		$res=$pmres[$module]->res;
		$GLOBALS["right_url"]=$pmres[$module]->right_url;
		$GLOBALS["left_url"]=$pmres[$module]->left_url;
		$roun=$pmres[$module]->roun;
		//$res->level=0;
		if($pmres[$module]->pup) $GLOBALS['pup']=$pmres[$module]->pup;
	}

	if((!empty($res) && $res->part!='') || (!empty($cpart) && isset($res->module_id))){	
		if($cpart==''){
			if(!isset($GLOBALS["part_cache4"][$res->part])){
				getrow($db,"SELECT * FROM main_part WHERE part_id=".$res->part,1,'main_part',__LINE__,__FILE__);
				$GLOBALS["part_cache4"][$res->part]=$db->Record;
			} else $db->Record=$GLOBALS["part_cache4"][$res->part];
		} else {
			if(is_numeric($cpart)){
				if(!isset($GLOBALS["part_cache4"][$cpart])){					
					getrow($db,"SELECT * FROM main_part WHERE part_id=".$cpart,1,'main_part',__LINE__,__FILE__);
					$GLOBALS["part_cache4"][$cpart]=$db->Record;
				} else $db->Record=$GLOBALS["part_cache4"][$cpart];
			} else getrow($db,"SELECT * FROM main_part WHERE part_module=".$res->module_id." AND part_sname='$cpart'",1,'main_part',__LINE__,__FILE__);
			if(!empty($GLOBALS["part_cache4"]) && !empty($db->Record["part_id"])) $GLOBALS["part_cache4"][$db->Record["part_id"]]=$db->Record;
		}
		if(!empty($db->Record) && isset($db->Record["part_name"]) && $db->Record["part_enable"]==1){
			$dbr=$db->Record;
			do_log("Shell part ".$dbr["part_name"].'/'.$dbr["part_sname"]);
			$force_auth=false;
			global $user;
			if(($dbr["part_auth"]==0 && !$user->super) || ($dbr["part_auth"]>0 && $dbr["part_auth"]!=$user->id)){
				if($dbr["part_auth"]==0) $force_auth='super'; else $force_auth=$dbr["part_auth"];
			}
			$r=backup_globals($force_auth);
			$GLOBALS["is_404"]=0; if(isset($GLOBALS["last_404"])){$GLOBALS["is_404"]=1;unset($GLOBALS["last_404"]);}
			if(!empty($dbr['part_404'])) $GLOBALS["is_404"]=1;
			$GLOBALS["spec_step"]=0;unset($GLOBALS["up2"]);unset($GLOBALS["up"]);//experemental
			$GLOBALS["module_url"]=$roun;			
			$GLOBALS["cur_ex"]=$res->module_ex;
			if(!empty($res->row)) $GLOBALS["cur_row"]=$res->row; //условие введено для того, чтобы оставлять информацию о вызвавшем модуль ROW (может повредить логике определения частей)
			if(!empty($master_owner)) $GLOBALS["cur_ex"].='?'.$master_owner;
			$GLOBALS["cur_module"]=$res->module_id;
			$GLOBALS["cur_table"]=$res->table_id;
			$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
			if(empty($GLOBALS["part_stop"])) $GLOBALS["part_stop"]='';
			$old_part_stop=$GLOBALS["part_stop"];

			$part_id=$dbr["part_id"];
			$part_major=$dbr["part_major"];	
			$part_body=$dbr["part_body"];		
			$part_parse=$dbr["part_parse"];
			$GLOBALS["cur_part"]=$part_id;
			if(!empty($dbr["part_ifrow"])){
				if(!isset($GLOBALS["parts_cow"][$part_id])){		
					$GLOBALS["cur_row"]=get_row_id(parse_var($dbr["part_ifrow"]));
					$GLOBALS["parts_cow"][$part_id]=$GLOBALS["cur_row"];
				} else $GLOBALS["cur_row"]=$GLOBALS["parts_cow"][$part_id];
			} else {
				$GLOBALS["parts_cow"][$part_id]=$res->row;
			}
			if(!$skip_shell){
				global $p_par;
				$cache_conf=get_cache_conf();
				global $tpl_static; $ts_back=$tpl_static; $tpl_static='';
				$butpl=start_part_c($part_id);
				global $puid; $bpuid=$puid; $puid=get_uid($p_par);


				if(!empty($op) && is_object($op) && !empty($op->put)){
					global $p_par;
					check_single($op->put,$GLOBALS["lex_func_sep"]);
					$par=prepend_param($op->put,$part_major,$part_id,0);
				} else {
					//global $p_par;
					$par=prepend_param(Array(),$part_major,$part_id,0);
					//$par=$p_par;
				}
				
				
				if(empty($GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]])) $GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=0;
				$old_skip=$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]];
				//$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=0;
			
				if(empty($GLOBALS["strip_spaces"][$GLOBALS["lvl2"]])) $GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=0;
				$old_strip=$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]];
				//$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=0;
		
				if(empty($GLOBALS["skip_space"][$GLOBALS["lvl2"]])) $GLOBALS["skip_space"][$GLOBALS["lvl2"]]=0;
				$old_skip2=$GLOBALS["skip_space"][$GLOBALS["lvl2"]];
				//$GLOBALS["skip_space"][$GLOBALS["lvl2"]]=0;
				
				if(empty($GLOBALS["skip_enter"][$GLOBALS["lvl2"]])) $GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=0;
				$old_skip3=$GLOBALS["skip_enter"][$GLOBALS["lvl2"]];
				//$GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=0;
				
				if(empty($GLOBALS["skip_empty"][$GLOBALS["lvl2"]])) $GLOBALS["skip_empty"][$GLOBALS["lvl2"]]=0;
				$old_skip4=$GLOBALS["skip_empty"][$GLOBALS["lvl2"]];
															
				
				start_part_shell($part_id);
				if($part_parse==0){
					// E5 tpl
					$result2=shell_tpl($part_body);		//Вот он! Запуск модуля
				} else if($part_parse==1){
					// PHP
					$result2=shell_php($part_body);
				}
				end_part_shell($part_id);


				$GLOBALS["skip_dblspace"][$GLOBALS["lvl2"]]=$old_skip;
				$GLOBALS["strip_spaces"][$GLOBALS["lvl2"]]=$old_strip;
				$GLOBALS["skip_space"][$GLOBALS["lvl2"]]=$old_skip2;
				$GLOBALS["skip_enter"][$GLOBALS["lvl2"]]=$old_skip3;
				$GLOBALS["skip_empty"][$GLOBALS["lvl2"]]=$old_skip4;
		
				//if(!empty($op) && is_object($op) && !empty($op->put)){
				$p_par=$par;
				//}					
					
				$puid=$bpuid;
				end_part_c($butpl);
				$tpl_static=$ts_back.$tpl_static;
				set_cache_conf($cache_conf);
				$GLOBALS["xbreak"]="";
				$result=concat_result($result,$result2,1);
				if($GLOBALS["part_stop"]==1  && $old_part_stop==0){ $GLOBALS["clear"]=1; $GLOBALS["exit"]=1; }
				if(!empty($GLOBALS["exit"])){
					del_part_history($res->level);
					return_globals($r);
					$GLOBALS["lvl"]--;					
					if(!$check_404){
						$r=check_404_page($res,$module2,$cpart,$skip_shell,$op);
						if(!empty($r)) return $r['data'];
					}
					return $result;
				}
			} else {
				//$result=parse_module($res->module_id,$var,$res->module_ex,'');//здесь последнее значение это LEFT элемент, который в данном случае не вызывается. т.е. вызвать часть модуля с параметром пока не удатстся
			}
			return_globals($r);

			if($GLOBALS["part_stop"]==1  && $old_part_stop==0){ $GLOBALS["clear"]=1; $GLOBALS["exit"]=1; }
		}
	} else if(is_numeric($cpart)){
		if(isset($xm[1])) $result=shell_part($cpart,$xm[1]);
		else $result=shell_part($cpart);
	}
	$roun=$roun2;
	$GLOBALS["lvl"]--;
	if(is_object($res)) del_part_history($res->level);
	if(!$check_404){
		$r=check_404_page($res,$module2,$cpart,$skip_shell,$op);
		if(!empty($r)) return $r['data'];
	}
	return $result;
}

function parse_component($type,&$op,&$str){
	$var[0]=$type;
	$var[1]=$op->data;
	global $p_par,$cur_col,$db;
	if($var[0]=='_function' || $var[0]=='_func') $type=0;
	if($var[0]=='_show') $type='1';
	if($var[0]=='_component') $type=2;
	if($var[0]=='_form') $type=3;
	$part_type='part_type=2';
	$part_proc=' AND part_proc='.$type;
	$part_mod='';
	if($var[0]=='_part'){
		$part_proc='';
		$part_type='part_type IN (0,1,3,4,5)';
		$part_mod=' AND part_module='.$GLOBALS["cur_module"];
	}
	getrow($db,"SELECT * FROM main_part WHERE ".$part_type.$part_proc.$part_mod." AND part_sname='".$var[1]."'",1,"main_part");
	if(!empty($db->Record)){
		$part_id=$db->Record["part_id"]; $part_major=$db->Record["part_major"];
		$cur_colB=$cur_col;
		$cur_col=$str;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$par=prepend_param(/*$pv*/$op->put,$part_major,$part_id,0);
		$res=shell_part($part_id,$GLOBALS['cur_ex'],$GLOBALS['cur_row']);
		$cur_col=$cur_colB;
		$p_par=$par;
	}
	return $res;
}

?>