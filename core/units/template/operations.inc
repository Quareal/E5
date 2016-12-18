<?php

$operations_a=array_flip(Array(' ','=>','=','->','*>','+>','+=','-=','/=','*='));

function get_equal_value(&$value,&$old_value,$operation){
	if($operation=='+=') return $old_value+$value;
	elseif($operation=='-=') return $old_value-$value;
	elseif($operation=='*=') return $old_value*$value;
	elseif($operation=='/=') return $old_value/$value;
	else return $value;
}

function check_operations(&$owner,&$owner_data,&$op,&$ops/*,&$continue*/){
	stf('check_operations');
	//$continue=false;
	global $db;
	/*if(!isset($op->next_operation)){
		$continue=true;
		return false;
	}*/
	if($op->next_operation==' '/* || $op->next_operation==','*/){
		$res='';
		//функции с вызовом параметров через пробел (function.func1 param1=value1, param2=value2) / component, function, func, show, form
		//а также ajax. и ajax2.
		//editrow, editrow2, editrow3, delrow, addrow, limit, filter, order, url, url2, url3, url4
		//flush X, cancel X, stand alone, skip dblspace, strip spaces, skip space, skip enter, return X,
		//искать по $op->main
		//меня терзают смутные сомненья, что это вообще будет работать
		//т.к. по идее система будет считать, что function(var1=val2 var2=val2) при обходе скобочек, что у них owner=0 и соответственно сделает ересь вроде function(var1(var2=val2))
		if($op->type==LEXER_SUB){
			//для случаев вроде [function.some_function var1=value]
			$lsub=&$op->sub[count($op->sub)-1];
			if(!isset($lsub->put)) $lsub->put=Array();
			//echo '<pre>';
			//var_dump($ops);
			//echo '</pre>';
			if(!empty($ops)) for($i=key($ops);$i<count($ops);$i++){
				//foreach($ops AS $top)
				$top=$ops[$i];
				$lsub->put[]=$top;
			}
			//var_dump($op->sub);
			
			//echo '<pre>';
			//var_dump($op->sub);
			//echo '</pre>';
			//exit;
			
			etf('check_operations');
			return parse_var($op->sub);
		} else {
			//для обычных случаев вроде [flush "varname"]
			if(!isset($op->data)) $op->data='';
			if(!isset($op->put)) $op->put=Array();
			if(!empty($ops)) /*foreach($ops AS $top)*/for($i=key($ops);$i<count($ops);$i++){
				$top=$ops[$i];
				$op->put[]=$top;
			}
			$cmd=find_cmd($op->data,CMD_BASE,$owner_data,$owner);
			$data=do_cmd($op,$ops,$cmd,$owner_data);
			etf('check_operations');
			return $data;
		}
	}
	if($op->next_operation=='=' || $op->next_operation=='+=' || $op->next_operation=='*=' || $op->next_operation=='-=' || $op->next_operation=='/='){
		$is_a=false;
		if($op->type==LEXER_SUB && !empty($op->sub) && count($op->sub)==2 && !empty($op->sub[0]->data) && !empty($op->sub[0]->next_operation) && $op->sub[0]->data=='param' && $op->sub[0]->next_operation=='.') $op=$op->sub[1];
		if($op->type==LEXER_SUB && !empty($op->sub) && !empty($op->sub[count($op->sub)-1]->arr)) $arr=$op->sub[count($op->sub)-1]->arr;
		else if(!empty($op->arr)) $arr=$op->arr;
		$var3=Array();

		$i=0;
		if(!empty($arr)){
			$is_a=true;
			foreach($arr AS $a){
				$var3[$i]=parse_var($a);
				if(is_array($a) && isset($a[0]) && is_object($a[0]) && isset($a[0]->type) &&
					(
						($a[0]->type==LEXER_NUM && empty($a[0]->data))
							||
						($a[0]->type==LEXER_STR && ($a[0]->data=='0' || $a[0]->data==''))
							||
						($a[0]->type==LEXER_CMD && empty($var3[$i]))
					)
				){
					$var3[$i]='%ZERO_KEY%';
					if($a[0]->data==='' && $a[0]->type==LEXER_STR) $var3[$i]='%ZERO_KEY2%';
					if($var3[$i]==='' && $a[0]->type==LEXER_SUB) $var3[$i]='%ZERO_KEY2%';
					elseif(empty($var3[$i]) && $a[0]->type==LEXER_SUB) $var3[$i]='%ZERO_KEY%';
				}
				if(is_object($var3[$i]) && isset($var3[$i]->id)) $var3[$i]=$var3[$i]->id;
				$i++;
			}
		}
		
		if($op->type==LEXER_SUB) $var=$op->sub[0]->data;
		else $var=$op->data;
		$t=array_pop($ops);
		$value=parse_var($t);

		$xz=-1;		
		if($op->type!=LEXER_SUB){
			if($is_a){
				if(!empty($var3)){
					for($i=0;$i<count($var3);$i++){
						if(!isset($z)) $z=&$GLOBALS['p_par'][$var];
						$var3b=$var3[$i];
						if(!empty($var3b) || $var3b=='%ZERO_KEY%' || $var3b=='%ZERO_KEY2%'){
							if($var3b=='%ZERO_KEY%') $var3b=0;
							elseif($var3b=='%ZERO_KEY2%') $var3b='';
							if(!is_string($z) || empty($z)){
								if(!isset($z[$var3b])) $z[$var3b]='';
								$z=&$z[$var3b];
							} else {
								$xz=$var3b;
								break;
							}
						} else $z=&$z[];
					}
				}
				if($xz!=-1) $z=substr($z,0,$xz).$value.substr($z,$xz+1);
				else $z=get_equal_value($value,$z,$op->next_operation);
			} else {
				$GLOBALS['p_par'][$var]=get_equal_value($value,$GLOBALS['p_par'][$var],$op->next_operation);
			}
			etf('check_operations');
			return false;
		}
		
		if($op->type==LEXER_SUB && $op->sub[0]->data=='glob'){
			$var=$op->sub[1]->data;
			if($is_a){
				if(!empty($var3)){
					for($i=0;$i<count($var3);$i++){
						if(!isset($z)) $z=&$GLOBALS['g_par'][$var];
						$var3b=$var3[$i];
						if(!empty($var3b) || $var3b=='%ZERO_KEY%'){
							if($var3b=='%ZERO_KEY%') $var3b=0;
							$z=&$z[$var3b];
							if(!isset($z[$var3b])) $z[$var3b]='';
						} else $z=&$z[];
					}
				}					
				$z=get_equal_value($value,$z,$op->next_operation);
			} else {
				$GLOBALS['g_par'][$var]=get_equal_value($value,$GLOBALS['g_par'][$var],$op->next_operation);
			}
			etf('check_operations');
			return false;
		}
		
		if($op->type==LEXER_SUB && ($op->sub[0]->data=='cook2' || $op->sub[0]->data=='scook')){
			$var=$op->sub[1]->data;
			if($is_a){
				//if(is_object($tmp2) && isset($tmp2->id)) $tmp2=$tmp2->id;//??
				if(!empty($_COOKIE[$var]) && is_string($_COOKIE[$var])){
					$d=$_COOKIE[$var];
					if(strstr($d,"\\\\")) $d=str_replace("\\\\",'!!-!!',$d);
					$d=str_replace("\\",'',$d);
					$d=str_replace('!!-!!',"\\\\",$d);
					$tvar=unserialize($d);
					if(!is_array($tvar)) $tvar=Array();
				}
				else $tvar=Array();
				if($is_a){
					$skip_add=false;
					if(!empty($var3)){
						for($i=0;$i<count($var3);$i++){
							if(!isset($z)) $z=&$tvar;
							$var3b=$var3[$i];
							if($value=='' && isset($z[$var3b]) && ($i==count($var3)-1)){
								unset($z[$var3b]);
								$skip_add=true;
								break;
							}
							if(!empty($var3b) || $var3b=='%ZERO_KEY%'){
								if($var3b=='%ZERO_KEY%') $var3b=0;
								$z=&$z[$var3b];
								if(!isset($z[$var3b])) $z[$var3b]='';
							} else $z=&$z[];
						}
					}
					if(!$skip_add) $z=get_equal_value($value,$z,$op->next_operation);
					if($skip_add) del_empty_array($tvar);
				}
				
			} else $tvar=get_equal_value($value,$tvar,$op->next_operation);
			if(!empty($tvar)){
				SetCookie($var,serialize($tvar),time()+3600*24*7,'/');
				$_COOKIE[$var]=serialize($tvar);
			} else {
				SetCookie($var,"",0,'/');
				if(isset($_COOKIE[$var])) unset($_COOKIE[$var]);
			}
			etf('check_operations');
			return false;				
		}
		if($op->type==LEXER_SUB && $op->sub[0]->data=='cook'){
			$var=$op->sub[1]->data;
			if($is_a){
				$sdata='';
				$z=&$_COOKIE[$var];
				for($i=0;$i<count($var3);$i++){
					if(empty($var3[$i])){
						if(is_array($z)) $c=count($z); else {$z=Array(); $c=0;}
						$sdata.='['.$c.']';
						$z=&$z[];
					} else {
						$sdata.='['.$var3[$i].']';
						$z=&$z[$var3[$i]];
					}
				}
				$z=get_equal_value($value,$z,$op->next_operation);
				SetCookie($var.$sdata,$value,time()+3600*24*7,'/');
			} else {
				SetCookie($var,$value,time()+3600*24*7,'/');
				$_COOKIE[$var]=get_equal_value($value,$_COOKIE[$var],$op->next_operation);
			}
			if(empty($_COOKIE[$var])){
				SetCookie($var,'',0,'/');
				if(isset($_COOKIE[$var])) unset($_COOKIE[$var]);
			}
			etf('check_operations');
			return false;
		}
		etf('check_operations');
		return false;
	}
	if($op->next_operation=='=>'){
		$var=$op->data;
		$tmp=explodeA('=>',$var,'[',']',1,'{','}',true);
		$is_a=false;
		if(!empty($op->arr)){
			$tmp2=parse_var($op->arr[0]);
			$is_a=true;
		}
		$x_id=parse_var($op);
		$id=explode(':',$x_id);
		if(!$x_id || !is_string($x_id) || !strstr($x_id,':') || !is_numeric($id[0]) || count($id)<2 || !is_numeric($id[1])){
			$nop1=new lexer_op();
			$nop1->type=LEXER_CMD;
			$nop1->data='col';
			$nop1->next_operation='.';
			$nop2=new lexer_op();
			$nop2->type=LEXER_CMD;
			$nop2->data='id';
			if($op->type==LEXER_SUB){
				$op->sub[count($op->sub)-1]->next_operation='.';
				$op->sub[count($op->sub)]=$nop1;
				$op->sub[count($op->sub)]=$nop2;
				//var_dump($op->sub);
				$x_id=parse_var($op->sub);
				//var_dump($x_id);
			} else {
				$op->next_operation='.';
				$nop=Array($op,$nop1,$nop2);
				$op=$nop;
				$x_id=parse_var($op);
			}
			//var_dump($op);
			//exit;			
		}
		if(!$x_id || !is_string($x_id) || !strstr($x_id,':')){ etf('check_operations'); return false; }
		$id=explode(':',$x_id);
		
		/*if(empty($id[1]) || !is_numeric($id[1]) || empty($id[0]) || !is_numeric($id[0])){
			if($op->type!=LEXER_SUB){
				$top=$op;
				$op->sub=Array($top);
				$op->type=LEXER_SUB;
			}
			$op->sub[0]->next_operation='.';
			$op->sub[]=create_op('col',LEXER_CMD,'.');
			$op->sub[]=create_op('id',LEXER_CMD,'.');
			$id=explode(':',parse_var(parse_var($op->sub)));
		}*/
		$icache=false;
		$value=array_pop($ops);
		$fop=first_deep_op($value);
		if(!empty($id)){				
			//if(strlen5($tmp[1])>0 && $tmp[1][strlen5($tmp[1])-1]=='~') $icache=true;
			if(isset($fop->mutator) && $fop->mutator==MUTATOR_SUP) $icache=true;
			$value=parse_var($value);
			if(is_object($value) && isset($value->id)) $value=$value->id;
			seek_rlink($id[1]);global $rlink;
			if(check_row($id[1],$rlink[$id[1]]->table,get_ex2($rlink[$id[1]]->tex),'edit',$rlink[$id[1]]->user,$rlink[$id[1]]->users,$id[3])){
				if(!$icache) del_cache('row',$id[1]);
				clear_rowval_cache($id[0],$id[1]);
				if(!$is_a && !empty($id[0]) && !empty($id[1])){
					getrow($db,"SELECT * FROM main_col WHERE col_id=$id[0] AND col_module=$id[3]",1,"main_col");
					$col=$db->Record;
					if($col["col_type"]==3){
						//getrow($db,'SELECT * FROM row_value WHERE value_row='.$id[1].' AND value_col='.$id[0].' AND value_table>0',3,'row_value',__LINE__,__FILE__);
						//if($db->Record["value_value"]!="" && $db->Record["value_value"]!=$value && file_exists(DOCUMENT_ROOT.$db->Record["value_value"])){
						//	$dbval=$db->Record["value_value"];
						//	del_file($dbval);
						//}
						//$nname=upload_file_col($col);
						//if($nname!=-1 && $nname) $value=$nname;
						set_col_file($value,$id[1],$col);
					} else {
					
						//if($col["col_type"]==3) $value=prepend_file($value);
						$value=prepend_value($value);
					
						$db->query('DELETE FROM row_value WHERE value_row='.$id[1].' AND value_col='.$id[0].' AND value_table>0',3,'row_value',__LINE__,__FILE__);
						$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
							VALUES ('.$id[3].','.$id[2].','.$id[0].','.$id[1].','."'".$value."'".')',3,'row_value',__LINE__,__FILE__);
					}
				} else if(isset($tmp2) && $tmp2==''){
					$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
						VALUES ('.$id[3].','.$id[2].','.$id[0].','.$id[1].','."'".$value."'".')',3,'row_value',__LINE__,__FILE__);				
				} else {
				         //
				}
				if(!empty($id[1]) && !empty($id[0])) rebuild_row_index($id[1],$id[0]);
			}
		}
		update_row_state($id[1]);
		
		etf('check_operations');
		return false;
	}
	if($op->next_operation=='->'){
		global $prepost;
		$value=parse_var(array_pop($ops));
		if($op->type==LEXER_SUB && !empty($op->sub) && $op->sub[0]->next_operation==':'){
			if(is_object($value) && isset($value->id)) $value=$value->id;				
			$var=parse_var($op->sub[0]);
			$var2=parse_var($op->sub[1]);
			$value=prepend_value($value);
			$prepost[$var]->val=$value;
			$prepost[$var]->sname=$var2;
			$prepost[$var]->autoload=false;
		} else {
			$var=parse_var($op);
			if(is_object($var) && isset($var->type) && $var->type=='cl'){
				$var=$var->sname;
			}
			if(is_object($value) && isset($value->id)) $value=$value->id;
			//$prepost[$var]->sname='';
			$value=prepend_value($value);
			$prepost[$var]->val=$value;
			$prepost[$var]->autoload=false;
		}
		etf('check_operations');
		return false;
	}
	if($op->next_operation=='*>'){
		global $g_par, $p_par;
		if($op->type!=LEXER_SUB/* && !empty($op->sub)*/){
			$var=&$p_par[$op->data];
		} else {
			if($op->sub[0]->data=='param') $var=&$p_par[$op->sub[1]->data];
			else if($op->sub[0]->data=='glob') $var=&$g_par[$op->sub[1]->data];
		}
		$value=array_pop($ops);
		if($value->type==LEXER_SUB && !empty($value->sub) && $value->sub[0]->next_operation==':'){
			$var[parse_var($value->sub[0])]=parse_var($value->sub[1]);
		} else {
			if(empty($var) || !is_array($var)) $var=Array();
			$var[]=parse_var($value);
		}
		etf('check_operations');
		return false;
	}
	if($op->next_operation=='+>'){
		global $prepost;
		$var=parse_var($op);
		$value=parse_var(array_pop($ops));
		if(is_object($value) && isset($value->id)) $value=$value->id;
		$value=prepend_value($value);
		if(empty($prepost[$var]->val)){
			$prepost[$var]->val=Array();
			$prepost[$var]->autoload=true;
		}
		if(!empty($prepost[$var]->val) && !is_array($prepost[$var]->val)){ $tmp=$prepost[$var]->val; $prepost[$var]->val=Array(); $prepost[$var]->val[$tmp]=1;}
		$prepost[$var]->val[$value]=1;
		etf('check_operations');
		return false;
	}
	//$continue=true;
	etf('check_operations');
}

?>