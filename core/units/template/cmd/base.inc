<?php

define('GROUP_MAIN_PART',1009);
$cmd_group[GROUP_MAIN_PART]=Array('title'=>'Текущая часть','pos'=>4);		
define('GROUP_CONTROL',1001);
$cmd_group[GROUP_CONTROL]=Array('title'=>'Управление шаблоном','pos'=>25);
define('GROUP_SYSTEM',1002);
$cmd_group[GROUP_SYSTEM]=Array('title'=>'Системные','pos'=>25);
define('GROUP_SPECIFIC',1003);
$cmd_group[GROUP_SPECIFIC]=Array('title'=>'Особенные','pos'=>25);
define('GROUP_OPERATIONS',1004);
$cmd_group[GROUP_OPERATIONS]=Array('title'=>'Операции','pos'=>25);
define('GROUP_FOR',1005);
$cmd_group[GROUP_FOR]=Array('title'=>'Цикл','pos'=>25);
define('GROUP_URL',1006);
$cmd_group[GROUP_URL]=Array('title'=>'URL','pos'=>25);
define('GROUP_VARS',1007);
$cmd_group[GROUP_VARS]=Array('title'=>'Переменные','pos'=>25);
define('GROUP_CONST',1008);
$cmd_group[GROUP_CONST]=Array('title'=>'Константы','pos'=>25);

$cmd['addrow'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $db;
		global $prepost,$f_action;
		$force=false;
		$icache=false;
		if(shift_op($op->put,'force')){
			$force=true;
		}
		if(shift_op($op->put,'icache')){
			$icache=true;
		}
		$oldfa=$f_action;
		if(!empty($data) && is_string($data) && strstr($data,':')) $table=$data;
		else $table=parse_var(array_shift($op->put));
		$x=get_normal_table($table);
		$table_owner=$x->owner;
		$table_id=$x->id;
		$module_ex=$x->ex;
		$table_ex=$x->tex;

		/*if(isset($table->id)){
			if(isset($table->own)) $table->owner=$table->own;
			if(!isset($table->owner)) $table->owner=0;
			if(strstr($table->id,':')){
				$tid=explode(':',$table->id);
				$table_id=$tid[0];
				$module_ex=$tid[1];
			} else {
				$table_id=$table->id;
			}
			if(isset($table->ex)){
				$module_ex=$table->ex;
				$table_ex=get_tex($table->owner,$module_ex,$table_id);
			}
			if(isset($table->tex)){
				$table_ex=$table->tex;
				$module_ex=get_ex2($table_ex);
			} else $table_ex=0;
			$table_owner=$table->owner;
		} else if(!is_object($table) && strstr($table,':')) {
			$table=explode(':',$table);
			$table_id=$table[0];
			$module_ex=$table[1];
			if(isset($table[4])) $table_ex=$table[4]; else $table_ex=0;
			$table_owner=$table[2];
		}*/
		$rsub=0;
		if(empty($table_ex) && !empty($table_owner)){
			getrow($db,'SELECT * FROM main_row WHERE row_id='.$table_owner,1,'main_row',__LINE__,__FILE__);
			$table_ex=$db->Record['row_ex'];
			$rsub=$db->Record['row_id'];//$db->Record['row_sub']; //? <- вообще не уверен что это к месту
			if(!$icache) del_cache('row',$rsub);
			//$table_ex=$table_owner; // Ох как я не уверен что именно родительский ROW становится экземпляром для данной таблицы (хотя логично)
		}
		if(!empty($table_id)){
			$tmp=load_cols_by_table($table_id);
		}
		if(empty($tmp)) return false;
		//protect
		$ex2=$module_ex;
		if(!empty($rsub)) $ex2=0;//рискованный ход, хотя если row_sub не пустое, то подразумевается, что эти значения являются из дочерней таблицы		
		
		//нужно проверять с этим table_owner (это вроде owner_id)
		if(empty($table_owner)) $chk=check_row(0,$table_id,$ex2,'add');
		else {
			$ex2=get_ex2($table_ex);//это оказывается необходимо			
			$chk=check_operation('add',0,$table_owner,$ex2,$table_id);
		}
		if(!$chk){
			$GLOBALS['cancel']='not enough privileges';
			return false;//true //былое
		}
		//end protect
		$module_id=$tmp[0]['col_module'];
		$user_field='';
		$url_field='';
		$cols=Array(); foreach($tmp AS $tm){
			$cols[$tm['col_sname']]='col'.$tm['col_id'];
			$cols[$tm['col_id']]='col'.$tm['col_id'];
			if($tm['col_type']==5) $user_field=$tm['col_sname'];
			if($tm['col_url']) $url_field=$tm['col_sname'];
		}
		if(!empty($prepost)) foreach($prepost AS $var=>$value) if(!empty($cols[$var]) || in_array($var,Array('user','url','major'))){
			if($var=='major'){
				$major_col=getrowval('SELECT major_col FROM main_table WHERE table_id='.$table_id,'major_col');
				if(!empty($major_col) && !empty($cols[$major_col])) $var=$major_col;
			}
			if($var=='user' && !empty($user_field)) $var=$user_field;
			if($var=='url' && !empty($url_field) && empty($cols['url'])) $var=$url_field;
			if(empty($cols[$var])) continue;
			if(!empty($value->sname)) $_POST[$cols[$var]]=get_ival($value->val,$value->sname,$cols[$var],$module_ex,$table_id);
			else $_POST[$cols[$var]]=$value->val;
		}

		$row_sub=0;
		if($table_owner!=0) $row_sub=seek_rowsub($table_owner,$table_id);
		if(!empty($table_owner)){
			global $rlink;
			seek_rlink($table_owner);
			if(empty($rlink[$table_owner])) return false;
			$owner_table=$rlink[$table_owner]->table;
			$owner_module=$rlink[$table_owner]->module;
			//getrow($db,'SELECT * FROM main_row WHERE row_id='.$table_owner,1,'main_row',__LINE__,__FILE__);
			//if(empty($db->Record)) return false;
			//$owner_table=$db->Record['row_table'];
			//$owner_module=$db->Record['row_module'];
		} else {$owner_table=0;$owner_module=0;}
	
		if(!$icache){
			del_cache('rows',$table_id.'.'.$table_ex);
		}
		
		//спорный момент //возможно тут стоит сделать удаление кеша всех родителей по вертикали
		//и надо уже из этих двух функций (этой и add_row) сделать одну
		if(!empty($row_sub)){
			del_cache('row',$row_sub);
		}
		
		if(empty($table_ex)){
			getrow($db,'SELECT * FROM ex_group WHERE ex_module='.$module_id.' AND ex_table='.$table_id.' AND ex_ex2='.$module_ex,1,'ex_group',__LINE__,__FILE__);
			if(empty($db->Record['ex_ex1'])){
				//echo $module_id.'|'.$table_id.'|'.$module_ex.'<br>';
				return false;
			}
			$table_ex=$db->Record['ex_ex1'];
		}
		
		global $user;
		$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_user, row_uin, modified_date, creation_date)
				VALUES ($module_id, $table_id, $table_ex, $row_sub, ".$user->id.", '".uuin()."', '".date('Y-m-d H:i:s')."', '".date('Y-m-d H:i:s')."')",3,'main_row',__LINE__,__FILE__);
		//getrow($db,'SELECT LAST_INSERT_ID() as sid');		
		//$sid=$db->Record['sid'];
		$sid=$db->last_insert_id();
		
		$GLOBALS['li']=$sid;
		if($owner_module==0)$owner_module=$module_id;
		if($owner_table==0)$owner_table=$table_id;
		getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex IN (".get_exes($table_owner,$table_ex).") AND row_table=$table_id AND owner_id=$table_owner AND owner_table=$owner_table AND ro_sub=$row_sub AND owner_module=$owner_module",1,'row_owner',__LINE__,__FILE__);
		if(!empty($db->Record['mid'])) $pos=$db->Record['mid']+1; else $pos=1;
		if(empty($id6)) $ro_sub=0; else $ro_sub=$id6;
		$db->query("INSERT INTO row_owner (ro_pos, ro_ex, row_id, ro_sub, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
				VALUES ($pos, $table_ex, $sid, $row_sub, $module_id, $table_id, $table_owner, $owner_table, $owner_module, ".$user->id.")",3,'row_owner',__LINE__,__FILE__);
		$GLOBALS['cancel']=false;
		
		$r=backup_globals();
		$GLOBALS['ex_ex2']=$module_ex;//вот косяк так косяк. вместо cur_ex надо указывать эту дрянь
		$GLOBALS['cur_module']=$module_id;
		$GLOBALS['f_action']='add';
		
		insert_values($table_id,$module_id,$sid,$force);
				
		return_globals($r);
		
		if($GLOBALS['cancel']) del_row($sid,1);

		if(!empty($prepost)) foreach($prepost AS $var=>$value) if(!empty($cols[$var])){
			if(isset($_POST[$cols[$var]])) unset($_POST[$cols[$var]]);
		}		
		
		global $prepost_array;
		if(!empty($prepost_array)) $prepost=array_pop($prepost_array); else $prepost=Array();
		$f_action=$oldfa;
		global $gsres, $rcache;
		$gsres=Array();
		$rcache=Array();		
		
		return false;
};
$cmd['addrow'][CMD_BASE]->result=CMD_NONE;
$cmd['addrow'][CMD_BASE]->visual=Array('title'=>'добавить','group'=>GROUP_OPERATIONS,'put'=>Array(
	0=>Array('title'=>'отключить обработчики','type'=>CMD_STATIC,'static'=>'force','req'=>0),
	1=>Array('title'=>'не очищать кеш','type'=>CMD_STATIC,'static'=>'icache','req'=>0),
	2=>Array('title'=>'таблица','type'=>CMD_TABLE,'req'=>1)
),'special'=>SP_SPACE);


$cmd['editrow_full'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $db,$prepost,$f_action,$rlink;
		$oldfa=$f_action; $f_action='edit';
		if(shift_op($op->put,'icache')){
			$i+=7;
		} else $icache=false;
		if(count($op->put)==2){
			$row=parse_var(array_pop($op->put));
			if(is_object($row)) $row=$row->id;
			$table=parse_var(array_shift($op->put));
			$x=get_normal_table($table);
			$table_owner=$x->owner;
			$table_id=$x->id;
			$module_ex=$x->ex;
			$table_ex=$x->tex;
			/*$table=explode(':',$table);
			$table_id=$table[0];
			$module_ex=$table[1];
			$table_owner=$table[2];
			$table_ex=$table[4];	*/
		} else {
			$row=parse_var(array_shift($op->put));
			if(is_object($row)) $row=$row->id;
			seek_rlink($row);
			$table_id=$rlink[$row]->table;
			$table_ex=$rlink[$row]->tex;
			$table_owner=$rlink[$row]->rsub;
			$module_ex=get_ex2($table_ex);
		}
		$tmp=getall($db,'SELECT * FROM main_col WHERE col_table='.$table_id,1,'main_col',__LINE__,__FILE__);
		if(empty($tmp)) return false;
		//protect
		seek_rlink($row);global $rlink;
		$table=$rlink[$row]->table;
		$user=$rlink[$row]->user;
		$rsub=$rlink[$row]->rsub;
		getrow($db,"SELECT * FROM ex_group WHERE ex_ex1="./*$db->Record["row_ex"]*/$rlink[$row]->tex,1,'ex_group',__LINE__,__FILE__);
		$ex=$db->Record['ex_ex2'];
		$ex2=$ex;
		if(!check_operation('edit',$row,$rsub,$ex2,$table)){
			$GLOBALS['cancel']='not enough privileges';
			return false;//true
		}
		//end protect
		$module_id=$tmp[0]['col_module'];
		$user_field='';
		$url_field='';
		$cols=Array(); $cols2=Array(); foreach($tmp AS $tm){
			$cols[$tm['col_sname']]='col'.$tm['col_id'];
			$cols[$tm['col_id']]='col'.$tm['col_id'];
			$cols2[$tm['col_sname']]=$tm['col_id'];
			if($tm['col_type']==5) $user_field=$tm['col_sname'];
			if($tm['col_url']) $url_field=$tm['col_sname'];
		}
		if(!empty($prepost)) foreach($prepost AS $var=>$value) if(!empty($cols[$var]) || in_array($var,Array('user','url','major'))){
			if($var=='major'){
				$major_col=getrowval('SELECT major_col FROM main_table WHERE table_id='.$table_id,'major_col');
				if(!empty($major_col) && !empty($cols[$major_col])) $var=$major_col;
			}
			if($var=='user' && !empty($user_field)) $var=$user_field;
			if($var=='url' && !empty($url_field) && empty($cols['url'])) $var=$url_field;
			if(empty($cols[$var])) continue;
			clear_rowval_cache($cols2[$var],$row);
			if(!empty($value->sname)) $_POST[$cols[$var]]=get_ival($value->val,$value->sname,$cols[$var],$module_ex,$table_id);
			else $_POST[$cols[$var]]=$value->val;			
		}
		if(!$icache) del_cache('row',$row);
		$backup=backup_row($row);
		$GLOBALS["backrow"]=$backup;
		$sid=$row;
		del_vals_pre($row,$table_id);
		$GLOBALS['cancel']=false;
		insert_values($table_id,$module_id,$sid);
		if($GLOBALS['cancel']){
			del_vals(' value_table!=0 AND value_row='.$sid);
			copy_vars($backup,$sid);
			del_row($backup,1);
		}
		if(!$GLOBALS['cancel']){
			update_row_state($sid);
			del_row($backup,1);
		}
		global $prepost_array;
		if(!empty($prepost_array)) $prepost=array_pop($prepost_array); else $prepost=Array();
		$f_action=$oldfa;
		return false;
};
$cmd['editrow_full'][CMD_BASE]->result=CMD_NONE;
$cmd['editrow2'][CMD_BASE]->alias=&$cmd['editrow_full'][CMD_BASE];
$cmd['editrow_full'][CMD_BASE]->visual=Array('title'=>'изменить (метод удаление+добавление)','group'=>GROUP_OPERATIONS,'put'=>Array(
		0=>Array('title'=>'не очищать кеш','type'=>CMD_STATIC,'static'=>'icache','req'=>0),
		1=>Array('title'=>'таблица','type'=>CMD_TABLE,'req'=>0),
		2=>Array('title'=>'объект','type'=>CMD_ROW,'req'=>1)
),'special'=>SP_SPACE);


$cmd['editrow'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $prepost,$rlink,$db;
		$force=true;
		if($op->data=='editrow3') $force=false;
		if(shift_op($op->put,'icache')) $force=false;
		if(!empty($data) && is_numeric($data)){
			$row=$data;
			if(is_object($row)) $row=$row->id;
			seek_rlink($row);
			$table_id=$rlink[$row]->table;
			$table_ex=$rlink[$row]->tex;
			$table_owner=$rlink[$row]->rsub;
			$module_ex=get_ex2($table_ex);			
		} else if(count($op->put)==2){
			$row=parse_var(array_pop($op->put));
			if(empty($row)) return false;
			if(is_object($row)) $row=$row->id;
			$table=parse_var(array_shift($op->put));
			/*$table=explode(':',$table);
			$table_id=$table[0];
			$module_ex=$table[1];
			$table_owner=$table[2];
			$table_ex=$table[4];*/
			$x=get_normal_table($table);
			$table_owner=$x->owner;
			$table_id=$x->id;
			$module_ex=$x->ex;
			$table_ex=$x->tex;
		} else {
			$row=parse_var(array_shift($op->put));
			if(empty($row)) return false;
			if(is_object($row)) $row=$row->id;
			seek_rlink($row);
			$table_id=$rlink[$row]->table;
			$table_ex=$rlink[$row]->tex;
			$table_owner=$rlink[$row]->rsub;
			$module_ex=get_ex2($table_ex);
		}
		
		//protect		
		seek_rlink($row); global $rlink;
		$table=$rlink[$row]->table;
		$table_id=$table;
		$user=$rlink[$row]->user;
		$rsub=$rlink[$row]->rsub;
		$tex=$rlink[$row]->tex;
		getrow($db,'SELECT * FROM ex_group WHERE  ex_ex1='.$tex,1,'ex_group',__LINE__,__FILE__);
		$ex=$db->Record['ex_ex2'];
		$ex2=$ex;
		if(!check_operation('edit',$row,$rsub,$ex2,$table)){
			$GLOBALS['cancel']='not enough privileges';
			return false;//true
		}
		//end protect
		$tmp=getall($db,'SELECT * FROM main_col WHERE col_table='.$table_id,1,'main_col',__LINE__,__FILE__);
		if(empty($tmp)) return false;
		$module_id=$tmp[0]['col_module'];
		del_cache('row',$row);
		$user_field='';
		$url_field='';
		$cols=Array(); foreach($tmp AS $tm){
			$cols[$tm['col_sname']]=$tm;
			$cols[$tm['col_id']]=$tm;
			if($tm['col_type']==5) $user_field=$tm['col_sname'];
			if($tm['col_url']) $url_field=$tm['col_sname'];
		}
		if(!empty($prepost)) foreach($prepost AS $var=>$value2)if(!empty($cols[$var]) || in_array($var,Array('user','url','major'))){
			$ignore_have=false;
			if($var=='major'){
				$major_col=getrowval('SELECT major_col FROM main_table WHERE table_id='.$table_id,'major_col');
				if(!empty($major_col) && !empty($cols[$major_col])) $var=$major_col;
			}
			if($var=='user' && !empty($user_field)) $var=$user_field;
			if($var=='url' && !empty($url_field) && empty($cols['url'])) $var=$url_field;
			if(empty($cols[$var])) continue;
			if(isset($value2->autoload) && !empty($value2->autoload)) $ignore_have=true;
			if(!isset($value2->val) || !is_array($value2->val)){
				$value3=Array($value2);
			} else {
				$value3=Array();
				foreach($value2->val AS $key=>$tmp){
					$value3[$key]->val=$key;
				}
			}
			$first=true;
			foreach($value3 AS $value){
				$col=$cols[$var]["col_id"];
				clear_rowval_cache($col,$row);
				if(!empty($value->sname)) $value=get_ival($value->val,$value->sname,$col,$module_ex,$table_id);
				else $value=$value->val;
				$db->query("SELECT * FROM row_value WHERE value_row=$row AND value_col=$col",1,'row_value',__LINE__,__FILE__);			
				$db->next_record();
				$have=$db->num_rows();
				if($have) $have2=$db->Record["value_value"]; else $have2='';
				if($cols[$var]["col_type"]==3 && !empty($_FILES[$var]) && !empty($_FILES[$var]["tmp_name"])){
					$value=upload_file_col($cols[$var],'',$value);
				}
				if($cols[$var]["col_type"]==3) $value=prepend_file($value);
				$value=prepend_value($value);
				if($cols[$var]["col_type"]==3 && $value=='' && $have){
					$value=$have2;//защита от косяка, когда при изменении фотографии она удаляется
				}
				if($ignore_have){
					//в этом случае проводится проверка, на наличие уже имеющихся связей
					$db->query("SELECT * FROM row_value WHERE value_row=$row AND value_col=$col AND value_value=$value",1,'row_value',__LINE__,__FILE__);
					$db->next_record();
					$have3=$db->num_rows();
				} else $have3=false;
				if(!$have3){
					if($have && $first && !$ignore_have){
						$db->query("UPDATE row_value SET value_value='$value' WHERE value_row=$row AND value_col=$col",3,'row_value',__LINE__,__FILE__);
					} else {
						$db->query("INSERT INTO row_value (value_value, value_row, value_col, value_module, value_table)
							VALUES('$value',$row,$col,$module_id,$table_id)",3,'row_value',__LINE__,__FILE__);
					}
				}
				$first=false;
			}
		}
			
		global $vcache,$vcache2, $globr, $prow, $prowval, $qc; unset($vcache); unset($vcache2); unset($globr); unset($prow); unset($prowval); unset($qs);
		
		$res='';
		if(empty($GLOBALS["cancel"]) && !empty($prepost) && !$force) foreach($prepost AS $var=>$value) if(!empty($cols[$var]) && !empty($cols[$var]["col_oninsert"])){
			$res=concat_result($res,on_col_edit($row,$cols[$var]["col_id"],$cols[$var]["col_oninsert"],$module_id,$ex2,$table));
		}
		$tdc=table_cache($table_id,1);
		if(!empty($tdc["table_onedit"]) && !$force){
			global $f_type;
			$oldft=$f_type;
			$f_type='onedit2';
			$b=backup_globals();
			$GLOBALS["cur_row"]=$row;
			$GLOBALS["current"]=$row;
			$GLOBALS["cur_ex"]=$ex2;
			$GLOBALS["cur_module"]=$module_id;
			$GLOBALS["cur_table"]=$table_id;
			$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$row;
			$res.=shell_tpl($tdc["table_onedit"],0);
			return_globals($b);
			$f_type=$oldft;
		}
		if(!empty($prepost)) foreach($prepost AS $var=>$value) if(!empty($cols[$var])){
			if(isset($_POST[$cols[$var]])) unset($_POST[$cols[$var]]);
		}
		global $prepost_array;
		if(!empty($prepost_array)) $prepost=array_pop($prepost_array); else $prepost=Array();
		update_row_state($row);
		
		if(empty($res)) return false;
		return $res;
};
$cmd['editrow'][CMD_BASE]->result=CMD_NONE;
$cmd['editrow3'][CMD_BASE]->alias=&$cmd['editrow'][CMD_BASE];
$cmd['editrow_handler'][CMD_BASE]->alias=&$cmd['editrow'][CMD_BASE];
$cmd['editrow'][CMD_BASE]->visual=Array('title'=>'изменить (без обработчиков)','group'=>GROUP_OPERATIONS,'put'=>Array(
		0=>Array('title'=>'не очищать кеш','type'=>CMD_STATIC,'static'=>'icache','req'=>0),
		1=>Array('title'=>'таблица','type'=>CMD_TABLE,'req'=>0),
		2=>Array('title'=>'объект','type'=>CMD_ROW,'req'=>1)
),'special'=>SP_SPACE);
$cmd['editrow_handler'][CMD_BASE]->visual=Array('title'=>'изменить (с обработчиками)','group'=>GROUP_OPERATIONS,'put'=>Array(
		0=>Array('title'=>'не очищать кеш','type'=>CMD_STATIC,'static'=>'icache','req'=>0),
		1=>Array('title'=>'таблица','type'=>CMD_TABLE,'req'=>0),
		2=>Array('title'=>'объект','type'=>CMD_ROW,'req'=>1)
),'special'=>SP_SPACE);


$cmd['delrow'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $db;
		if(!empty($data) && is_numeric($data)) $row=$data;
		else $row=parse_var(array_shift($op->put));
		if(empty($row)) return false;
		if(is_object($row)) $row=$row->id;
		if(!is_numeric($row) && strstr($row,':')){
			$row=explode(':',$row);
			if(count($row)>=3){
				$row=$row[2];
			} else return false;
		}
		seek_rlink($row);global $rlink;
		if(empty($rlink[$row])) return false;
		$table=$rlink[$row]->table;
		$user=$rlink[$row]->user;
		$rsub=$rlink[$row]->rsub;
		getrow($db,'SELECT * FROM ex_group WHERE ex_ex1='.$rlink[$row]->tex,1,'ex_group',__LINE__,__FILE__);
		$ex=$db->Record['ex_ex2'];
		$tex=$db->Record['ex_ex1'];
		$ex2=$ex;
		if(check_operation('del',$row,$rsub,$ex2,$table)){//заменил table2 на table
			del_row($row);
		} else {
			$GLOBALS['cancel']='not enough privileges';
			return false;
		}
		global $gsres; unset($gsres);
		global $rcache; unset($rcache);
		return false;
};
$cmd['delrow'][CMD_BASE]->result=CMD_NONE;
$cmd['delrow'][CMD_BASE]->visual=Array('title'=>'удалить','group'=>GROUP_OPERATIONS,'put'=>Array(
		0=>Array('title'=>'объект','type'=>CMD_ROW,'req'=>1),
),'special'=>SP_SPACE);


$cmd['order'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//поддержка ^
		if($op->put[0]->type==LEXER_SUB && !empty($op->put[0]->sub) && count($op->put[0]->sub)==2 && isset($op->put[0]->sub[0]->next_operation) && $op->put[0]->sub[0]->next_operation=='^'){
			$op->put[0]->sub[0]->next_operation=' ';
			$op->put[0]->sub[1]->mutator=MUTATOR_SUP;
			array_splice($op->put,0,1,$op->put[0]->sub);
		}
		$rows=parse_var(array_shift($op->put));		
		$name=array_shift($op->put);
		$fop=first_deep_op($name);
		$in_param=false;
		if(isset($fop->mutator) && $fop->mutator==MUTATOR_SUP) $in_param=1;
		$name=parse_var($name);
		
		$aop=array_shift($op->put);
		if($aop->type==LEXER_SUB && isset($aop->sub[0]->next_operation) && $aop->sub[0]->next_operation==':'){
			$col=parse_var($aop->sub[0]);
			$sort=parse_var($aop->sub[1]);
		} else {
			$col=parse_var($aop);
			$sort='ASC';
		}
		row_order($rows,$name,$col,$sort,$in_param);
		return false;
};
$cmd['order'][CMD_BASE]->result=CMD_NONE;
$cmd['order'][CMD_BASE]->visual=Array('title'=>'порядок','hidden'=>1,'put'=>Array(
		0=>Array('title'=>'объекты','type'=>CMD_ARRAY,'req'=>1),
		1=>Array('title'=>'имя переменной','type'=>CMD_STRING,'req'=>1,'sup'=>1,'sup_text'=>'присваивать результат в переменную'),
		2=>Array('title'=>'','type'=>CMD_DOUBLE,'sub'=>Array(
			0=>Array('title'=>'поле для сортировки','type'=>STRING_COLNAMES_ORDER,'req'=>1),
			1=>Array('title'=>'тип сортировки','type'=>CMD_STRING,'list'=>Array('ASK'=>'По возрастанию','DESC'=>'По убыванию','RAND'=>'Случайная'),'req'=>0)
		))
),'special'=>SP_SPACE);


$cmd['filter'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//поддержка ^
		if(empty($op->put) && !empty($ops)){
			if(isset($ops[key($ops)+1])) $op->put=$ops[key($ops)+1]->put;
			else $op->put=$ops[key($ops)]->put;
			$ops[key($ops)]->put=Array();
			if(isset($ops[key($ops)+1])){
				$ops[key($ops)+1]->put=Array();
				set_deep_mutator($ops[key($ops)+1],MUTATOR_SUP);
				array_unshift($op->put,$ops[key($ops)+1]);
			}
			set_deep_mutator($ops[key($ops)],MUTATOR_SUP);
			array_unshift($op->put,$ops[key($ops)]);
		}
		if($op->put[0]->type==LEXER_SUB && !empty($op->put[0]->sub) && count($op->put[0]->sub)==2 && isset($op->put[0]->sub[0]->next_operation) && $op->put[0]->sub[0]->next_operation=='^'){
			$op->put[0]->sub[0]->next_operation=' ';
			$op->put[0]->sub[1]->mutator=MUTATOR_SUP;
			array_splice($op->put,0,1,$op->put[0]->sub);
		}
		$rows=array_shift($op->put);
		$fop=first_deep_op($rows);
		$seek_sub=false;
		if(isset($fop->mutator) && $fop->mutator==MUTATOR_SUP) $seek_sub=true;
		$rows=parse_var($rows);
		$name=parse_var(array_shift($op->put));
		$if=array_shift($op->put);
		$fop=first_deep_op($if);
		if(isset($fop->mutator) && $fop->mutator==MUTATOR_SUP) $in_param=true;		
		else $in_param=false;
		row_filter($rows,$name,$if,$in_param,$seek_sub);
		return false;
};
$cmd['filter'][CMD_BASE]->result=CMD_NONE;
$cmd['filter'][CMD_BASE]->visual=Array('title'=>'фильтр','hidden'=>1,'put'=>Array(
		0=>Array('title'=>'объекты','type'=>CMD_ARRAY,'req'=>1,'sup'=>1,'sup_text'=>'учитывать потомков'),
		1=>Array('title'=>'имя переменной','type'=>CMD_STRING,'req'=>1,'sup'=>1,'sup_text'=>'присваивать результат в переменную'),
		2=>Array('title'=>'условие','type'=>CMD_IF,'req'=>1)
),'special'=>SP_SPACE);


$cmd['limit'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//поддержка ^
		if($op->put[0]->type==LEXER_SUB && !empty($op->put[0]->sub) && count($op->put[0]->sub)==2 && isset($op->put[0]->sub[0]->next_operation) && $op->put[0]->sub[0]->next_operation=='^'){
			$op->put[0]->sub[0]->next_operation=' ';
			$op->put[0]->sub[1]->mutator=MUTATOR_SUP;
			array_splice($op->put,0,1,$op->put[0]->sub);
		}
		$rows=parse_var(array_shift($op->put));
		$name=array_shift($op->put);
		$fop=&$name;//first_deep_op($name);
		$in_param=false;
		if(isset($fop->mutator) && $fop->mutator==MUTATOR_SUP) $in_param=true;		
		$name=parse_var($name);
		$limit=array_shift($op->put);
		row_limit($rows,$name,$limit->sub[0],$limit->sub[1],$in_param);
		return false;
};
$cmd['limit'][CMD_BASE]->result=CMD_NONE;
$cmd['limit'][CMD_BASE]->visual=Array('title'=>'обрезка','hidden'=>1,'put'=>Array(
		0=>Array('title'=>'объекты','type'=>CMD_ARRAY,'req'=>1),
		1=>Array('title'=>'имя переменной','type'=>CMD_STRING,'req'=>1,'sup'=>1,'sup_text'=>'присваивать результат в переменную'),
		2=>Array('title'=>'','type'=>CMD_DOUBLE,'sub'=>Array(
			0=>Array('title'=>'начало','type'=>CMD_STRING,'req'=>1),
			1=>Array('title'=>'конец','type'=>CMD_STRING,'req'=>1)
		))
),'special'=>SP_SPACE);


$cmd['url'][CMD_BASE]->process=function(&$data,&$op,&$ops,$type=1){
		global $forward;
		if($op->data=='url2') $type=2;
		if($op->data=='url3') $type=3;
		if($op->data=='url4') $type=4;
		if($type==1 && empty($op->put)) return $forward;
		//не ясна ситуация с массивами в $_GET
		$oldforw=$forward;
		check_single($op->put,Array(' '));
		$var[1]=parse_var($op->put[0]);
		if(!empty($op->put[1])) $var[2]=parse_var($op->put[1]);
		$forw=explode('?',$forward);
		if(empty($forw[1])) $get=Array(); else parse_str($forw[1],$get);
		if(!empty($var[2])) $get[$var[1]]=$var[2];
		$forward=$forw[0];
		$first=true;
		foreach($get AS $vr=>$value) if(
		  (($type==1 || $type==2 || $type==4) && (!empty($var[2]) || $var[1]!=$vr))
		  || ($type==3 && $var[1]!=$vr)
                ){
			if(!$first) $forward.='&'; else $forward.='?';$first=false;
			if(is_object($value) && isset($value->id)) $value=$value->id;
			if(!empty($value)) $forward.=$vr.'='.urlencode($value);
			else $forward.=$vr.'=';
		}
		if($type==4){
			$res=$forward;
			$forward=$oldforw;
			return $res;
		}
		if($type==2) return $forward; else return '';
};
$cmd['url'][CMD_BASE]->result=CMD_STRING;
$cmd['url2'][CMD_BASE]->alias=&$cmd['url'][CMD_BASE];
$cmd['url3'][CMD_BASE]->alias=&$cmd['url'][CMD_BASE];
$cmd['url4'][CMD_BASE]->alias=&$cmd['url'][CMD_BASE];
$cmd['url'][CMD_BASE]->visual=Array('title'=>'текущий URL','group'=>GROUP_URL,'result'=>CMD_NONE,'put'=>Array(
		0=>Array('title'=>'название новой переменной','type'=>CMD_STRING,'req'=>0),
		1=>Array('title'=>'значение новой переменной','type'=>CMD_STRING,'req'=>0,'prevreq'=>1)
),'special'=>SP_SPACE);
$cmd['url2'][CMD_BASE]->visual=Array('title'=>'Установить переменную в текущий URL и вывести его','group'=>GROUP_URL,'result'=>CMD_NONE,'put'=>Array(
		0=>Array('title'=>'название новой переменной','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'значение новой переменной','type'=>CMD_STRING,'req'=>1)
),'special'=>SP_SPACE);
$cmd['url3'][CMD_BASE]->visual=Array('title'=>'Удалить переменную из URL','group'=>GROUP_URL,'result'=>CMD_NONE,'put'=>Array(
		0=>Array('title'=>'название переменной','type'=>CMD_STRING,'req'=>1)
),'special'=>SP_SPACE);
$cmd['url4'][CMD_BASE]->visual=Array('title'=>'Вывести URL с установленной переменной, но не сохранять её','result'=>CMD_NONE,'group'=>GROUP_URL,'put'=>Array(
		0=>Array('title'=>'название переменной','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'значение переменной','type'=>CMD_STRING,'req'=>1)
),'special'=>SP_SPACE);


$cmd['exit'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$GLOBALS['exit']=1;
		return '';
};
$cmd['exit'][CMD_BASE]->result=CMD_NONE;
$cmd['exit'][CMD_BASE]->visual=Array('title'=>'выход','group'=>GROUP_CONTROL);


$cmd['pass'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$GLOBALS['pass']=1;
		return '';
};
$cmd['pass'][CMD_BASE]->result=CMD_NONE;
$cmd['pass'][CMD_BASE]->visual=Array('title'=>'пропустить перебор вложений','group'=>GROUP_FOR);


$cmd['break'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$GLOBALS['break']=1;
		return '';
};
$cmd['break'][CMD_BASE]->result=CMD_NONE;
$cmd['break'][CMD_BASE]->visual=Array('title'=>'выход из перебора','group'=>GROUP_FOR);


$cmd['xbreak'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$GLOBALS['xbreak']=1;
		return '';
};
$cmd['xbreak'][CMD_BASE]->result=CMD_NONE;
$cmd['xbreak'][CMD_BASE]->visual=Array('title'=>'выход из текущей части','group'=>GROUP_CONTROL);


$cmd['cancel'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($op->put) && $op->put[0]->type==LEXER_CMD && $op->put[0]->data=='silent'){
			$GLOBALS['cancel']=-1;
			return '';
		}
		if(!empty($op->put)){
			$GLOBALS['cancel']=parse_var($op->put);
			return '';
		}
		$GLOBALS['cancel']=1;
		return '';
};
$cmd['cancel'][CMD_BASE]->result=CMD_NONE;
$cmd['cancel'][CMD_BASE]->visual=Array('title'=>'отмена','special'=>SP_SPACE,'group'=>GROUP_CONTROL,'put'=>Array(
		0=>Array('title'=>'Отменить выполнение текущего обработчика без причины','type'=>CMD_STATIC,'static'=>'silent','req'=>0),
		1=>Array('title'=>'Причина','type'=>CMD_STRING,'req'=>0)
));


$cmd['continue'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($op->put)) $GLOBALS['continue']=parse_var($op->put);
		else $GLOBALS['continue']=1;
		return '';
};
$cmd['continue'][CMD_BASE]->result=CMD_NONE;
$cmd['continue'][CMD_BASE]->visual=Array('title'=>'перейти к следующему этапу перебора','group'=>GROUP_FOR,'put'=>Array(
		0=>Array('title'=>'Количество пропущенных этапов','type'=>CMD_STRING,'req'=>0)
));


$cmd['stand'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($op->put) && $op->put[0]->data=='alone'){
			$GLOBALS['part_stop']=1;
			return '';
		}
};
$cmd['stand'][CMD_BASE]->result=CMD_NONE;
$cmd['stand'][CMD_BASE]->visual=Array('title'=>'Вывести текущую часть отдельно','special'=>SP_SPACE,'group'=>GROUP_CONTROL,'put'=>Array(
		0=>Array('title'=>'','hidden'=>1,'type'=>CMD_STATIC,'static'=>'alone','default'=>'1')
));


$cmd['hard_cache'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$GLOBALS['hard_cache']=1;
		return '';
};
$cmd['hard_cache'][CMD_BASE]->result=CMD_NONE;
$cmd['hard_cache'][CMD_BASE]->visual=Array('title'=>'Жёсткое кеширование','special'=>SP_SPACE,'group'=>GROUP_CONTROL);


$cmd['start_operation'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $prepost,$prepost_array;
		if(empty($prepost)) $prepost=Array();
		$prepost_array[]=$prepost;
		$prepost=Array();
		return '';
};
$cmd['start_operation'][CMD_BASE]->result=CMD_NONE;
$cmd['start_operation'][CMD_BASE]->visual=Array('title'=>'Подготовка перед добавлением или изменением','group'=>GROUP_OPERATIONS,'position'=>100);


$cmd['skip'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($op->put) && $op->put[0]->data=='dblspace'){
			$GLOBALS['skip_dblspace'][$GLOBALS['lvl2']]=1;
			return '';
		}
		if(!empty($op->put) && $op->put[0]->data=='space'){
			$GLOBALS['skip_space'][$GLOBALS['lvl2']]=1;
			$GLOBALS['skip_space2'][$GLOBALS['lvl']]=1;
			return '';
		}
		if(!empty($op->put) && $op->put[0]->data=='enter'){
			$GLOBALS['skip_enter'][$GLOBALS['lvl2']]=1;
			$GLOBALS['skip_enter2'][$GLOBALS['lvl']]=1;
			return '';
		}
		if(!empty($op->put) && $op->put[0]->data=='empty'){
			$GLOBALS['skip_empty'][$GLOBALS['lvl2']]=1;
			$GLOBALS['skip_empty2'][$GLOBALS['lvl']]=1;
			return '';
		}
};
$cmd['skip'][CMD_BASE]->result=CMD_NONE;
$cmd['skip'][CMD_BASE]->visual=Array('title'=>'пропускать','group'=>GROUP_CONTROL,'special'=>SP_SPACE,'put'=>Array(
		0=>Array('title'=>'Что пропускать?','type'=>CMD_STATIC,'list'=>Array(
			'dblspace'=>'Двойной пробел',
			'space'=>'Пробелы',
			'enter'=>'Вертикальные пробелы',
			'empty'=>'Пустые строки'
		))
));


$cmd['strip'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($op->put) && $op->put[0]->data=='spaces'){
			$GLOBALS['strip_spaces'][$GLOBALS['lvl2']]=1;
			return '';
		}
};
$cmd['strip'][CMD_BASE]->result=CMD_NONE;
$cmd['strip'][CMD_BASE]->visual=Array('title'=>'Убрать пробелы с начала и конца шаблона','special'=>SP_SPACE,'group'=>GROUP_CONTROL,'put'=>Array(
		0=>Array('title'=>'','hidden'=>1,'type'=>CMD_STATIC,'static'=>'spaces','default'=>'1')
));


$cmd['clear'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$GLOBALS['clear']=1;
		return '';
};
$cmd['clear'][CMD_BASE]->result=CMD_NONE;
$cmd['clear'][CMD_BASE]->visual=Array('title'=>'Очистить буфер вывода текущей части до этой позиции','group'=>GROUP_CONTROL);


$cmd['#404'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		header('HTTP/1.0 404 Not Found');
		global $tpl_static,$tpl_header;
		$tpl_static.='[#header("HTTP/1.0 404 Not Found")]';
		if(empty($tpl_header)) $tpl_header=Array();
		$tpl_header[]='HTTP/1.0 404 Not Found';
		return '';
};
$cmd['#404'][CMD_BASE]->result=CMD_NONE;
$cmd['#404'][CMD_BASE]->visual=Array('title'=>'Отправить ошибку 404','group'=>GROUP_SPECIFIC);


$cmd['flush_cache'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		flush_cache();
		return '';
};
$cmd['flush_cache'][CMD_BASE]->result=CMD_NONE;
$cmd['flush_cache'][CMD_BASE]->visual=Array('title'=>'очистить кеш текущего рендера','group'=>GROUP_SPECIFIC);


$cmd['next_cron'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $limit_cron;
		$limit_cron++;
};
$cmd['next_cron'][CMD_BASE]->result=CMD_NONE;
$cmd['next_cron'][CMD_BASE]->visual=Array('title'=>'перейти к следующему заданию таймера','condition'=>IF_CRON,'group'=>GROUP_SPECIFIC);


$cmd['signal'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);//?
		$tmp[0]=parse_var($op->put[0]);
		if(!empty($op->put[1])) $tmp[1]=parse_var($op->put[1]); else $tmp[1]=0;
		if(!empty($op->put[2])) $tmp[2]=parse_var($op->put[2]); else $tmp[2]='';
		$tmp[2]=parse_var($op->put[2]);
		add_news($tmp[0],$tmp[2],$tmp[1]);
};
$cmd['signal'][CMD_BASE]->result=CMD_NONE;
$cmd['signal'][CMD_BASE]->visual=Array('title'=>'Оповещение','group'=>GROUP_SPECIFIC,'put'=>Array(
		0=>Array('title'=>'Сообщение','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Пользователь или группа','type'=>CMD_USER_GROUP,'req'=>0),
		2=>Array('title'=>'Адрес ссылки','type'=>CMD_STRING,'req'=>0)
));


$cmd['flush'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);//?
		$v=parse_var($op->put[0]);
		global $p_par;
		if(isset($p_par[$v])) unset($p_par[$v]);
		return '';
};
$cmd['flush'][CMD_BASE]->result=CMD_NONE;
$cmd['flush'][CMD_BASE]->visual=Array('title'=>'обнулить переменную','group'=>GROUP_SPECIFIC,'put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>CMD_STRING,'req'=>1)
));

$cmd['return'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);//?
		$v=parse_var($op->put[0]);
		$GLOBALS['clear']=1;
		$GLOBALS['xbreak']=1;
		if(is_object($v) || is_array($v)) $GLOBALS["last_return"]=$v; else $GLOBALS["last_return"]='';
		return $v;
};
$cmd['return'][CMD_BASE]->result=CMD_NONE;
$cmd['return'][CMD_BASE]->visual=Array('title'=>'вернуть результат','group'=>GROUP_CONTROL,'put'=>Array(
		0=>Array('title'=>'результат','type'=>CMD_MIXED,'req'=>1)
),'special'=>SP_SPACE);


$cmd['cmd'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return parse_var($op->put);
};
$cmd['cmd'][CMD_BASE]->result=CMD_MIXED;
$cmd['cmd'][CMD_BASE]->visual=Array('title'=>'выполнить операцию','group'=>GROUP_OPERATIONS,'put'=>Array(
		0=>Array('title'=>'Операция','type'=>CMD_CMD,'req'=>1)
));


$cmd['if'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$x=parse_var($op->put[0]);
		if(isset($op->put[1]) && $x) return parse_var($op->put[1]);
		if(isset($op->put[2]) && !$x) return parse_var($op->put[2]);
		return $x;
};
$cmd['if'][CMD_BASE]->result=CMD_MIXED;
$cmd['if'][CMD_BASE]->visual=Array('title'=>'выполнить условие','group'=>GROUP_OPERATIONS,'put'=>Array(
		0=>Array('title'=>'Условие','type'=>CMD_IF,'req'=>1),
		1=>Array('title'=>'Действие при выполнении условия','type'=>CMD_CMD,'req'=>0),
		2=>Array('title'=>'Действие при невыполнении условия','type'=>CMD_CMD,'req'=>0)
));


$cmd['remind_form'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($op->put)){
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			$x=parse_var($op->put[0]);
			if(empty($x)) $x=10;
			if(!empty($op->put[1])){
				$y=parse_var($op->put[0]);
			} else $y=false;
		} else $x=10;
		return user_remind_form($x,$y);
};
$cmd['remind_form'][CMD_BASE]->result=CMD_STRING;
$cmd['remind_form'][CMD_BASE]->visual=Array('title'=>'Форма восстановления пароля','group'=>GROUP_SPECIFIC, 'put'=>Array(
		0=>Array('title'=>'Стойкость (кол-во символов)','type'=>CMD_STRING,'req'=>0),
		1=>Array('title'=>'Отправить новый пароль на почту','type'=>CMD_LOGICAL,'req'=>0)
));


$cmd['step'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['step'];
};
$cmd['step'][CMD_BASE]->result=CMD_STRING;
$cmd['step'][CMD_BASE]->visual=Array('title'=>'Уровень вложенности текущего цикла','group'=>GROUP_FOR,'result'=>STRING_NUM);


$cmd['cancel?'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(empty($GLOBALS['cancel'])) $GLOBALS['cancel']='';
		return $GLOBALS['cancel'];
};
$cmd['cancel?'][CMD_BASE]->result=CMD_STRING;
$cmd['cancel?'][CMD_BASE]->visual=Array('title'=>'Причина отмены','group'=>GROUP_VARS);


$cmd['subdomain'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!isset($GLOBALS['auto_subdomain'])) $GLOBALS['auto_subdomain']='';
		return $GLOBALS['auto_subdomain'];
};
$cmd['subdomain'][CMD_BASE]->result=CMD_STRING;
$cmd['subdomain'][CMD_BASE]->visual=Array('title'=>'Поддомен','group'=>GROUP_URL);


$cmd['subdomain_point'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!isset($GLOBALS['auto_subdomain'])) $GLOBALS['auto_subdomain']='';
		$res=$GLOBALS['auto_subdomain'];
		if(!empty($res)) return $res.'.'; else return $res;
};
$cmd['subdomain_point'][CMD_BASE]->result=CMD_STRING;
$cmd['subdomain2'][CMD_BASE]->alias=&$cmd['subdomain_point'][CMD_BASE];
$cmd['subdomain_point'][CMD_BASE]->visual=Array('title'=>'Поддомен (с точкой на конце)','group'=>GROUP_URL);


$cmd['host'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['host'];
};
$cmd['host'][CMD_BASE]->result=CMD_STRING;
$cmd['host'][CMD_BASE]->visual=Array('title'=>'Хост (http+host+base_url+folder+/)','group'=>GROUP_URL);


$cmd['host_strip'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['host2'];
};
$cmd['host_strip'][CMD_BASE]->result=CMD_STRING;
$cmd['host2'][CMD_BASE]->alias=&$cmd['host_strip'][CMD_BASE];
$cmd['host_strip'][CMD_BASE]->visual=Array('title'=>'Хост (http+host+base_url+folder)','group'=>GROUP_URL);


$cmd['uprefix'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$r=$GLOBALS['host2'];
		if(!empty($r) && $r[strlen($r)-1]!='/') $r.='/';
		if(!empty($GLOBALS['module_url'])){
			if($GLOBALS['module_url'][strlen($GLOBALS['module_url'])-1]=='/') return $r.$GLOBALS['module_url'];
			else return $r.$GLOBALS['module_url'].'/';
		} else return $r;
};
$cmd['uprefix'][CMD_BASE]->result=CMD_STRING;
$cmd['uprefix'][CMD_BASE]->visual=Array('title'=>'URL расположения модуля','group'=>GROUP_URL,'pos'=>0);


$cmd['http_host'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $_SERVER['HTTP_HOST'];
};
$cmd['http_host'][CMD_BASE]->result=CMD_STRING;
$cmd['host3'][CMD_BASE]->alias=&$cmd['http_host'][CMD_BASE];
$cmd['http_host'][CMD_BASE]->visual=Array('title'=>'HTTP_HOST сервера','group'=>GROUP_URL);


$cmd['host_major'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['host4'];
};
$cmd['host_major'][CMD_BASE]->result=CMD_STRING;
$cmd['host4'][CMD_BASE]->alias=&$cmd['host_major'][CMD_BASE];
$cmd['host_major'][CMD_BASE]->visual=Array('title'=>'Домен и папка основной зоны','group'=>GROUP_URL);


$cmd['host_xname'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['host5'];
};
$cmd['host_xname'][CMD_BASE]->result=CMD_STRING;
$cmd['host5'][CMD_BASE]->alias=&$cmd['host_xname'][CMD_BASE];
$cmd['host_xname'][CMD_BASE]->visual=Array('title'=>'XName национального домена','group'=>GROUP_URL);


$cmd['host_nofolder'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['host6'];
};
$cmd['host_nofolder'][CMD_BASE]->result=CMD_STRING;
$cmd['host6'][CMD_BASE]->alias=&$cmd['host_nofolder'][CMD_BASE];
$cmd['host_nofolder'][CMD_BASE]->visual=Array('title'=>'Хост (http+host+base_root)','group'=>GROUP_URL);


$cmd['host_auto_subdomain'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!isset($GLOBALS['host7'])){
			if(!isset($GLOBALS['auto_subdomain'])) $GLOBALS['auto_subdomain']='';
			$res=$GLOBALS['auto_subdomain'];
			if(!empty($res)) $res.='.';
			//$GLOBALS['host7']=str_replace('http://','http://'.$res.$GLOBALS['host']);
			$GLOBALS['host7']=str_replace($GLOBALS['prot'].'://',$GLOBALS['prot'].'://'.$res,$GLOBALS['host']);
		}
		return $GLOBALS['host7'];
};
$cmd['host_auto_subdomain'][CMD_BASE]->result=CMD_STRING;
$cmd['host7'][CMD_BASE]->alias=&$cmd['host_auto_subdomains'][CMD_BASE];
$cmd['host_auto_subdomain'][CMD_BASE]->visual=Array('title'=>'Хост (http+auto_subdomain+host+base_root+folder+/)','group'=>GROUP_URL);


$cmd['base_dir'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['base_root'];
};
$cmd['base_dir'][CMD_BASE]->result=CMD_STRING;
$cmd['base_dir'][CMD_BASE]->visual=Array('title'=>'Имя папки, в которую установлена система','group'=>GROUP_URL);


$cmd['rel_dir'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['relative'];
};
$cmd['rel_dir'][CMD_BASE]->result=CMD_STRING;
$cmd['rel_dir'][CMD_BASE]->visual=Array('title'=>'Относительны путь (base_root + folder)','group'=>GROUP_URL);


$cmd['g_url'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['g_url'];
};
$cmd['g_url'][CMD_BASE]->result=CMD_STRING;
$cmd['g_url'][CMD_BASE]->visual=Array('title'=>'Запрошенный URL (без зоны, начинается не с /)','group'=>GROUP_URL);


$cmd['a_url'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($_GET['id'])) $tid=$_GET['id']; else if(!empty($_POST['id'])) $tid=$_POST['id']; else $tid=0;
		if(!empty($_GET['id2'])) $tid2=$_GET['id2']; else if(!empty($_POST['id2'])) $tid2=$_POST['id2']; else $tid2=0;
		if(!empty($_GET['id3'])) $tid3=$_GET['id3']; else if(!empty($_POST['id3'])) $tid3=$_POST['id3']; else $tid3=0;
		if(!empty($_GET['id4'])) $tid4=$_GET['id4']; else if(!empty($_POST['id4'])) $tid4=$_POST['id4']; else $tid4=0;
		if(!empty($_GET['id5'])) $tid5=$_GET['id5']; else if(!empty($_POST['id5'])) $tid5=$_POST['id5']; else $tid5=0;
		if(!empty($_GET['id6'])) $tid6=$_GET['id6']; else if(!empty($_POST['id6'])) $tid6=$_POST['id6']; else $tid6=0;
		if(!empty($_GET['id7'])) $tid7=$_GET['id7']; else if(!empty($_POST['id7'])) $tid7=$_POST['id7']; else $tid7=0;
		return 'mod_table?id='.$tid.'&id2='.$tid2.'&id3='.$tid3.'&id4='.$tid4.'&id5='.$tid5.'&id6='.$tid6.'&id7='.$tid7;
};
$cmd['a_url'][CMD_BASE]->result=CMD_STRING;
$cmd['a_url'][CMD_BASE]->visual=Array('title'=>'URL текущего кабинета администратора','group'=>GROUP_URL);


$cmd['a_url_ampersand'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($_GET['id'])) $tid=$_GET['id']; else if(!empty($_POST['id'])) $tid=$_POST['id']; else $tid=0;
		if(!empty($_GET['id2'])) $tid2=$_GET['id2']; else if(!empty($_POST['id2'])) $tid2=$_POST['id2']; else $tid2=0;
		if(!empty($_GET['id3'])) $tid3=$_GET['id3']; else if(!empty($_POST['id3'])) $tid3=$_POST['id3']; else $tid3=0;
		if(!empty($_GET['id4'])) $tid4=$_GET['id4']; else if(!empty($_POST['id4'])) $tid4=$_POST['id4']; else $tid4=0;
		if(!empty($_GET['id5'])) $tid5=$_GET['id5']; else if(!empty($_POST['id5'])) $tid5=$_POST['id5']; else $tid5=0;
		if(!empty($_GET['id6'])) $tid6=$_GET['id6']; else if(!empty($_POST['id6'])) $tid6=$_POST['id6']; else $tid6=0;
		if(!empty($_GET['id7'])) $tid7=$_GET['id7']; else if(!empty($_POST['id7'])) $tid7=$_POST['id7']; else $tid7=0;
		return 'mod_table?id='.$tid.'&amp;id2='.$tid2.'&amp;id3='.$tid3.'&amp;id4='.$tid4.'&amp;id5='.$tid5.'&amp;id6='.$tid6.'&amp;id7='.$tid7;
};
$cmd['a_url_ampersand'][CMD_BASE]->result=CMD_STRING;
$cmd['a_url2'][CMD_BASE]->alias=&$cmd['a_url_ampersand'][CMD_BASE];
$cmd['a_url_ampersand'][CMD_BASE]->visual=Array('title'=>'URL текущего кабинета администратора (разделитель - &amp;)','group'=>GROUP_URL);


$cmd['a_inputs'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($_GET['id'])) $tid=$_GET['id']; else if(!empty($_POST['id'])) $tid=$_POST['id']; else $tid=0;
		if(!empty($_GET['id2'])) $tid2=$_GET['id2']; else if(!empty($_POST['id2'])) $tid2=$_POST['id2']; else $tid2=0;
		if(!empty($_GET['id3'])) $tid3=$_GET['id3']; else if(!empty($_POST['id3'])) $tid3=$_POST['id3']; else $tid3=0;
		if(!empty($_GET['id4'])) $tid4=$_GET['id4']; else if(!empty($_POST['id4'])) $tid4=$_POST['id4']; else $tid4=0;
		if(!empty($_GET['id5'])) $tid5=$_GET['id5']; else if(!empty($_POST['id5'])) $tid5=$_POST['id5']; else $tid5=0;
		if(!empty($_GET['id6'])) $tid6=$_GET['id6']; else if(!empty($_POST['id6'])) $tid6=$_POST['id6']; else $tid6=0;
		if(!empty($_GET['id7'])) $tid7=$_GET['id7']; else if(!empty($_POST['id7'])) $tid7=$_POST['id7']; else $tid7=0;
		$res='';
		if(!empty($tid)) $res.='<input type="hidden" name="id" value="'.$tid.'">';
		if(!empty($tid2)) $res.='<input type="hidden" name="id2" value="'.$tid2.'">';
		if(!empty($tid3)) $res.='<input type="hidden" name="id3" value="'.$tid3.'">';
		if(!empty($tid4)) $res.='<input type="hidden" name="id4" value="'.$tid4.'">';
		if(!empty($tid5)) $res.='<input type="hidden" name="id5" value="'.$tid5.'">';
		if(!empty($tid6)) $res.='<input type="hidden" name="id6" value="'.$tid6.'">';
		if(!empty($tid7)) $res.='<input type="hidden" name="id7" value="'.$tid7.'">';
		return $res;
};
$cmd['a_inputs'][CMD_BASE]->result=CMD_STRING;
$cmd['a_inputs'][CMD_BASE]->visual=Array('title'=>'Необходимые input hidden для реализации формы для кабинета администратора','group'=>GROUP_URL);


$cmd['r_url'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($_SERVER['REQUEST_URI'])) $res=$_SERVER['REQUEST_URI']; else $res='';
		return $res;
};
$cmd['r_url'][CMD_BASE]->result=CMD_STRING;
$cmd['r_url'][CMD_BASE]->visual=Array('title'=>'REQUEST URI сервера','group'=>GROUP_URL);


$cmd['ajax?'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($GLOBALS['from_ajax'])) $res=true; else $res=false;
		return $res;
};
$cmd['ajax?'][CMD_BASE]->result=CMD_STRING;
$cmd['ajax?'][CMD_BASE]->visual=Array('title'=>'Вызов идёт через Ajax?','group'=>GROUP_VARS);


$cmd['zone_specify'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($GLOBALS['zone_specify'])) $res=$GLOBALS['zone_specify']; else $res='';
		return $res;
};
$cmd['zone_specify'][CMD_BASE]->result=CMD_STRING;
$cmd['zone_specify'][CMD_BASE]->visual=Array('title'=>'Специфическая переменная зоны','group'=>GROUP_VARS);


$cmd['zone_email'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($GLOBALS['system_email'])) $res=$GLOBALS['system_email']; else $res='';
		return $res;
};
$cmd['zone_email'][CMD_BASE]->result=CMD_STRING;
$cmd['zone_email'][CMD_BASE]->visual=Array('title'=>'Email зоны','group'=>GROUP_VARS);


$cmd['post_files'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($_FILES)) $res=$_FILES; else $res=Array();
		return $res;
};
$cmd['post_files'][CMD_BASE]->result=CMD_ARRAY;
$cmd['post_file'][CMD_BASE]->alias=&$cmd['post_files'][CMD_BASE];
$cmd['post_files'][CMD_BASE]->visual=Array('title'=>'Массив отправленных файлов','group'=>GROUP_VARS);


$cmd['fullurl'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['fullurl2'];
};
$cmd['fullurl'][CMD_BASE]->result=CMD_STRING;
$cmd['fullurl'][CMD_BASE]->visual=Array('title'=>'полный URL (http+host+request_uri)','group'=>GROUP_URL);


$cmd['lefturl'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($GLOBALS["left_url_b"])) return $res=implode('/',$GLOBALS['left_url_b']); else
		if(!empty($GLOBALS["left_url"])) return $res=implode('/',$GLOBALS['left_url']); else return '';
};
$cmd['lefturl'][CMD_BASE]->result=CMD_STRING;
$cmd['lefturl'][CMD_BASE]->visual=Array('title'=>'Оставшаяся от URL-разбора часть URL','group'=>GROUP_URL);


$cmd['index'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['index'];
};
$cmd['index'][CMD_BASE]->result=CMD_STRING;
$cmd['index'][CMD_BASE]->visual=Array('title'=>'Номер этапа текущего перебора','group'=>GROUP_FOR,'result'=>STRING_NUM);


$cmd['case_url'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['case_url'];
};
$cmd['case_url'][CMD_BASE]->result=CMD_STRING;
$cmd['case_url'][CMD_BASE]->visual=Array('title'=>'Неразобранная часть URL (использовать для условий подбора частей)','condition'=>IF_PART_CASE,'group'=>GROUP_URL);


$cmd['var'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['cur_var'];
};
$cmd['var'][CMD_BASE]->result=CMD_MIXED;
$cmd['var'][CMD_BASE]->visual=Array('title'=>'Индекс текущего элемента цикла','group'=>GROUP_FOR,'result'=>STRING_NUM);


$cmd['first?'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $res=($GLOBALS['index']==0  && $GLOBALS['step']==0);
};
$cmd['first?'][CMD_BASE]->result=CMD_STRING;
$cmd['first?'][CMD_BASE]->visual=Array('title'=>'Это первый перебор перебора?','group'=>GROUP_FOR);


$cmd['last?'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['last'];
};
$cmd['last?'][CMD_BASE]->result=CMD_STRING;
$cmd['last?'][CMD_BASE]->visual=Array('title'=>'Это последний элемент перебора?','group'=>GROUP_FOR);


$cmd['count'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['count'];
};
$cmd['count'][CMD_BASE]->result=CMD_STRING;
$cmd['count'][CMD_BASE]->visual=Array('title'=>'Количество элементов в переборе','group'=>GROUP_FOR,'result'=>STRING_NUM);


$cmd['modurl'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['module_url'];
};
$cmd['modurl'][CMD_BASE]->result=CMD_STRING;
$cmd['modurl'][CMD_BASE]->visual=Array('title'=>'URL текущего модуля','group'=>GROUP_URL);


$cmd['modurl_slash'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($GLOBALS['module_url'])){
			if($GLOBALS['module_url'][strlen($GLOBALS['module_url'])-1]=='/') return $GLOBALS['module_url'];
			else return $GLOBALS['module_url'].'/';
		} else return '';
};
$cmd['modurl_slash'][CMD_BASE]->result=CMD_STRING;
$cmd['modurl2'][CMD_BASE]->alias=&$cmd['modurl_slash'][CMD_BASE];
$cmd['modurl_slash'][CMD_BASE]->visual=Array('title'=>'URL текущего модуля ( / в конце)','group'=>GROUP_URL);


$cmd['min'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['for_min'];
};
$cmd['min'][CMD_BASE]->result=CMD_STRING;
$cmd['min'][CMD_BASE]->visual=Array('title'=>'Начальное значение для перебора','condition'=>IF_TREE_NUMERIC,'group'=>GROUP_FOR,'result'=>STRING_NUM);


$cmd['max'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['for_max'];
};
$cmd['max'][CMD_BASE]->result=CMD_STRING;
$cmd['max'][CMD_BASE]->visual=Array('title'=>'Конечное значение для перебора','condition'=>IF_TREE_NUMERIC,'group'=>GROUP_FOR,'result'=>STRING_NUM);


$cmd['false'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//return 0;
		return false;
};
$cmd['false'][CMD_BASE]->result=CMD_STRING;
$cmd['false'][CMD_BASE]->visual=Array('title'=>'Ложь','group'=>GROUP_CONST);


$cmd['pi'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//return 0;
		return pi();
};
$cmd['pi'][CMD_BASE]->result=CMD_STRING;
$cmd['pi'][CMD_BASE]->visual=Array('title'=>'Число Пи','group'=>GROUP_CONST);


$cmd['true'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//return 1;
		return true;
};
$cmd['true'][CMD_BASE]->result=CMD_STRING;
$cmd['true'][CMD_BASE]->visual=Array('title'=>'Правда','group'=>GROUP_CONST);


$cmd['value'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['for_value'];
};
$cmd['value'][CMD_BASE]->result=CMD_MIXED;
$cmd['value'][CMD_BASE]->visual=Array('title'=>'Текущий элемент перебора, его значение','group'=>GROUP_FOR);


$cmd['zone_name'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['zone'][$GLOBALS['za']]['zone_name'];
};
$cmd['zone_name'][CMD_BASE]->result=CMD_STRING;
$cmd['zone_name'][CMD_BASE]->visual=Array('title'=>'Название зоны','group'=>GROUP_VARS);


$cmd['zone_type'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['zone'][$GLOBALS['za']]['zone_module'];
};
$cmd['zone_type'][CMD_BASE]->result=CMD_STRING;
$cmd['zone_type'][CMD_BASE]->visual=Array('title'=>'Тип зоны','group'=>GROUP_VARS);


$cmd['import'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(empty($GLOBALS['import'])) return '';
		else return $GLOBALS['import'];
};
$cmd['import'][CMD_BASE]->result=CMD_STRING;
$cmd['import'][CMD_BASE]->visual=Array('title'=>'Содержимое файла импорта','condition'=>IF_IMPORT,'group'=>GROUP_VARS);


$cmd['f_action'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(empty($GLOBALS['f_action'])) return '';
		else return $GLOBALS['f_action'];
};
$cmd['f_action'][CMD_BASE]->result=CMD_STRING;
$cmd['f_action'][CMD_BASE]->visual=Array('title'=>'Тип текущего действия (добавление/изменение)','group'=>GROUP_VARS);


$cmd['f_type'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(empty($GLOBALS['f_type'])) return '';
		else return $GLOBALS['f_type'];
};
$cmd['f_type'][CMD_BASE]->result=CMD_STRING;
$cmd['f_type'][CMD_BASE]->visual=Array('title'=>'Тип текущего обработчика','group'=>GROUP_VARS);


$cmd['#13'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//if(!empty($GLOBALS['skip_enter2'][$GLOBALS['lvl']]) || seek_to_top($GLOBALS["skip_enter2"],$GLOBALS['lvl'])) return '#bcd3!';
		if(!empty($GLOBALS['skip_enter'][$GLOBALS['lvl2']]) || seek_to_top($GLOBALS["skip_enter"],$GLOBALS['lvl2'])) return '#bcd3!';
		else return chr(13);
};
$cmd['#13'][CMD_BASE]->result=CMD_STRING;
$cmd['#13'][CMD_BASE]->visual=Array('title'=>'Вертикальный пробел #13','group'=>GROUP_CONST);


$cmd['#10'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//if(!empty($GLOBALS['skip_enter2'][$GLOBALS['lvl']]) || seek_to_top($GLOBALS["skip_enter2"],$GLOBALS['lvl'])) return '#bcd2!';
		if(!empty($GLOBALS['skip_enter'][$GLOBALS['lvl2']]) || seek_to_top($GLOBALS["skip_enter"],$GLOBALS['lvl2'])) return '#bcd2!';
		else return chr(10);
};
$cmd['#10'][CMD_BASE]->result=CMD_STRING;
$cmd['#10'][CMD_BASE]->visual=Array('title'=>'Вертикальный пробел #10','group'=>GROUP_CONST);


$cmd['#'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//if(!empty($GLOBALS['skip_space2'][$GLOBALS['lvl']]) || seek_to_top($GLOBALS["skip_space2"],$GLOBALS['lvl'])) return '#bcd1!';
		if(!empty($GLOBALS['skip_space'][$GLOBALS['lvl2']]) || seek_to_top($GLOBALS["skip_space2"],$GLOBALS['lvl2'])) return '#bcd1!';
		else return " ";
};
$cmd['#'][CMD_BASE]->result=CMD_STRING;
$cmd['#'][CMD_BASE]->visual=Array('title'=>'Пробел','group'=>GROUP_CONST);


$cmd['is_404'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return $GLOBALS['is_404'];
};
$cmd['is_404'][CMD_BASE]->result=CMD_STRING;
$cmd['is_404'][CMD_BASE]->visual=Array('title'=>'Это страница 404?','group'=>GROUP_VARS);


$cmd['season'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$m=date('m');
		if(($m>=1 && $m<=2) || $m==12) $res='winter'; if($m>=3 && $m<=5) $res='spring'; if($m>=6 && $m<=8) $res='summer'; if($m>=9 && $m<=11) $res='autumn';
		return $res;
};
$cmd['season'][CMD_BASE]->result=CMD_STRING;
$cmd['season'][CMD_BASE]->visual=Array('title'=>'Сезон','group'=>GROUP_VARS);


$cmd['season_num'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$m=date('m');
		if(($m>=1 && $m<=2) || $m==12) $res='0'; if($m>=3 && $m<=5) $res='1'; if($m>=6 && $m<=8) $res='2'; if($m>=9 && $m<=11) $res='3';
		return $res;
};
$cmd['season_num'][CMD_BASE]->result=CMD_STRING;
$cmd['season2'][CMD_BASE]->alias=&$cmd['season_num'][CMD_BASE];
$cmd['season_num'][CMD_BASE]->visual=Array('title'=>'Сезон (число 0-3)','group'=>GROUP_VARS);


$cmd['is_value'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 0;
};
$cmd['is_value'][CMD_BASE]->result=CMD_STRING;
$cmd['is_value'][CMD_BASE]->visual=Array('title'=>'Тип поля "значение/текст"','group'=>GROUP_CONST);


$cmd['is_link'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 1;
};
$cmd['is_link'][CMD_BASE]->result=CMD_STRING;
$cmd['is_link'][CMD_BASE]->visual=Array('title'=>'Тип поля "ссылка на объект"','group'=>GROUP_CONST);


$cmd['is_boolean'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 2;
};
$cmd['is_boolean'][CMD_BASE]->result=CMD_STRING;
$cmd['is_boolean'][CMD_BASE]->visual=Array('title'=>'Тип поля "логический"','group'=>GROUP_CONST);


$cmd['is_file'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 3;
};
$cmd['is_file'][CMD_BASE]->result=CMD_STRING;
$cmd['is_file'][CMD_BASE]->visual=Array('title'=>'Тип поля "файл"','group'=>GROUP_CONST);


$cmd['is_module'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 4;
};
$cmd['is_module'][CMD_BASE]->result=CMD_STRING;
$cmd['is_module'][CMD_BASE]->visual=Array('title'=>'Тип поля "ссылка на модуль"','group'=>GROUP_CONST);


$cmd['is_user'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 5;
};
$cmd['is_user'][CMD_BASE]->result=CMD_STRING;
$cmd['is_user'][CMD_BASE]->visual=Array('title'=>'Тип поля "ссылка на пользователя"','group'=>GROUP_CONST);


$cmd['is_folder'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 6;
};
$cmd['is_folder'][CMD_BASE]->result=CMD_STRING;
$cmd['is_folder'][CMD_BASE]->visual=Array('title'=>'Тип поля "папка"','group'=>GROUP_CONST);


$cmd['is_group'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 7;
};
$cmd['is_group'][CMD_BASE]->result=CMD_STRING;
$cmd['is_group'][CMD_BASE]->visual=Array('title'=>'Тип поля "ссылка на группу пользовтелей"','group'=>GROUP_CONST);


$cmd['is_admin'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return -1;
};
$cmd['is_admin'][CMD_BASE]->result=CMD_STRING;
$cmd['is_admin'][CMD_BASE]->visual=Array('title'=>'Тип зоны "административный кабинет"','group'=>GROUP_CONST);


$cmd['is_normal'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return 1;
};
$cmd['is_normal'][CMD_BASE]->result=CMD_STRING;
$cmd['is_normal'][CMD_BASE]->visual=Array('title'=>'Тип зоны "обычный сайт"','group'=>GROUP_CONST);


$cmd['is_admin?'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return ($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1);
};
$cmd['is_admin?'][CMD_BASE]->result=CMD_STRING;
$cmd['is_admin?'][CMD_BASE]->visual=Array('title'=>'Это административный кабинет?','group'=>GROUP_VARS);


$cmd['admin_row_url'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(!empty($GLOBALS["admin_row_url"])) $res=$GLOBALS["admin_row_url"]; else $res='';
		return $res;
};
$cmd['admin_row_url'][CMD_BASE]->result=CMD_STRING;
$cmd['admin_row_url'][CMD_BASE]->visual=Array('title'=>'URL текущей сущности в административном кабинете','group'=>GROUP_URL);


$cmd['form_protect'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$add='';
		if(empty($GLOBALS["check_for_xss"])) $add='<input type="hidden" name="check_xss" value="1">';
		$res='<input type="hidden" name="form_protect" value="'.uuid().'">'.$add;
		return $res;
};
$cmd['form_protect'][CMD_BASE]->result=CMD_NONE;
$cmd['form_protect'][CMD_BASE]->visual=Array('title'=>'Защита формы от повторной отправки','group'=>GROUP_SPECIFIC);


$cmd['ctbl'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($GLOBALS['ctbl'])){
			return $GLOBALS['ctbl'];//prepend_type($GLOBALS['ctbl'],'tbl');
		} else return '';
};
$cmd['ctbl'][CMD_BASE]->result=CMD_TABLE;
$cmd['ctbl'][CMD_BASE]->visual=Array('title'=>'Текущая таблица в административном кабинете','group'=>GROUP_VARS);


$cmd['a_rows'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($GLOBALS["a_rows"])) $res=$GLOBALS["a_rows"];
		else $res=Array();
		return $res;
};
$cmd['a_rows'][CMD_BASE]->result=CMD_ARRAY;
$cmd['a_rows'][CMD_BASE]->result_long=CMD_ROW;
$cmd['a_rows'][CMD_BASE]->visual=Array('title'=>'Объекты текущей таблицы (все)','condition'=>IF_TABLE_BOTTOM,'group'=>GROUP_VARS);


$cmd['a_rows_sel'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		if(isset($GLOBALS["a_rows2"])) $res=$GLOBALS["a_rows2"];
		else $res=Array();
		return $res;
};
$cmd['a_rows_sel'][CMD_BASE]->result=CMD_ARRAY;
$cmd['a_rows_sel'][CMD_BASE]->result_long=CMD_ROW;
$cmd['a_rows2'][CMD_BASE]->alias=&$cmd['a_rows_sel'][CMD_BASE];
$cmd['a_rows_sel'][CMD_BASE]->visual=Array('title'=>'Объекты текущей таблицы (выведенные на текущей странице)','condition'=>IF_TABLE_BOTTOM,'group'=>GROUP_VARS);


$cmd['get'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//if(!empty($ops) && cmd_separator($ops)=='.') $op=array_shiftA($ops);
		if(!empty($op->put)){
			$x=parse_var($op->put);
		} else {
			$op=array_shiftA($ops);
			$x=$op->data;
		}
		if(isset($_GET[$x])) return $_GET[$x]; else return '';
};
$cmd['get'][CMD_BASE]->result=CMD_STRING;
$cmd['get'][CMD_BASE]->visual=Array('title'=>'Переменная из GET','put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_VARS,'special'=>SP_POINT);


$cmd['post'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		//if(!empty($ops) && cmd_separator($ops)=='.') $op=array_shiftA($ops);
		if(!empty($op->put)){
			$x=parse_var($op->put);
		} else {
			$op=array_shiftA($ops);
			$x=$op->data;
		}
		if(isset($_FILES[$x])) return $_FILES[$x]["name"];
		else if(isset($_POST[$x])) return $_POST[$x];
		else return '';
};
$cmd['post'][CMD_BASE]->result=CMD_STRING;
$cmd['post'][CMD_BASE]->visual=Array('title'=>'Переменная из POST','put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_VARS,'special'=>SP_POINT);


$cmd['stat'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['stat'][CMD_BASE]->result=CMD_STAT;
$cmd['stat'][CMD_BASE]->visual=Array('title'=>'Функции статистики','group'=>GROUP_SPECIFIC,'disable_insert'=>1);


$cmd['cow'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cur_row;
		if(is_object($cur_row) && isset($cur_row->id)) $cur_row=$cur_row->id;
		return prepend_type($cur_row,'row');
};
$cmd['cow'][CMD_BASE]->result=CMD_ROW;
$cmd['cow'][CMD_BASE]->visual=Array('title'=>'Текущий объект (определённый по URL)','condition'=>IF_COW,'pos'=>1);


$cmd['deeper_cow'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $last_cow;
		if(is_object($last_cow) && isset($last_cow->id)) $last_cow=$last_cow->id;
		return prepend_type($last_cow,'row');
};
$cmd['deeper_cow'][CMD_BASE]->result=CMD_ROW;
$cmd['deeper_cow'][CMD_BASE]->visual=Array('title'=>'Объект, определённый через URL (не ограниченный текущим модулем)','pos'=>1,'group'=>GROUP_SPECIFIC);


$cmd['cur'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $current,$cur_type;
		if(is_object($current) && isset($current->id)) $current=$current->id;
		if(is_object($current) && isset($current->type)) $cur_type_this=$current->type;
		else if(empty($cur_type)) $cur_type_this='row';
		else $cur_type_this=$cur_type;
		return prepend_type($current,$cur_type_this);
		//return $current;
};
$cmd['cur'][CMD_BASE]->result=CMD_CURTYPE;
$cmd['cur_id'][CMD_BASE]->alias=&$cmd['cur'][CMD_BASE];
$cmd['cur'][CMD_BASE]->visual=Array('title'=>'Текущий объект (элемент цикла)','result'=>CMD_MIXED/*,'condition'=>IF_CUR*/,'group'=>GROUP_FOR,'pos'=>2);
$cmd['current'][CMD_BASE]->alias=&$cmd['cur'][CMD_BASE];
$cmd['current'][CMD_BASE]->visual=Array('title'=>'Текущий объект (элемент цикла)','result'=>CMD_ROW,'condition'=>IF_CUR,'pos'=>2);


$cmd['li'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $li;
		if(is_object($li) && isset($li->id)) $li=$li->id;
		return prepend_type($li,'row');
};
$cmd['li'][CMD_BASE]->result=CMD_ROW;
$cmd['li'][CMD_BASE]->visual=Array('title'=>'Последний добавленный элемент','group'=>GROUP_VARS);


$cmd['lu'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $lu;
		if(empty($lu)) return false;
		else return prepend_type($lu,'usr');
};
$cmd['lu'][CMD_BASE]->result=CMD_USER;
$cmd['lu'][CMD_BASE]->visual=Array('title'=>'Последний добавленный пользователь','group'=>GROUP_VARS);


$cmd['backrow'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $backrow;
		if(empty($backrow)) return false;
		else return prepend_type($backrow,'row');
};
$cmd['backrow'][CMD_BASE]->result=CMD_ROW;
$cmd['backrow'][CMD_BASE]->visual=Array('title'=>'Доступ к предыдущим значениям объекта в момент его изменения','group'=>GROUP_VARS);


$cmd['cex'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cur_ex;
		return prepend_type($cur_ex,'ex');
};
$cmd['cex'][CMD_BASE]->result=CMD_EX;
$cmd['cex'][CMD_BASE]->visual=Array('title'=>'Текущий экземпляр модуля','condition'=>IF_MODULE,'pos'=>3);


$cmd['cm'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cur_module;
		return prepend_type($cur_module,'md');
};
$cmd['cm'][CMD_BASE]->result=CMD_MODULE;
$cmd['cm'][CMD_BASE]->visual=Array('title'=>'Текущий модуль','condition'=>IF_MODULE,'pos'=>4);


$cmd['zone'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $zi;
		return prepend_type($zi,'zn');
};
$cmd['zone'][CMD_BASE]->result=CMD_ZONE;
$cmd['zone'][CMD_BASE]->visual=Array('title'=>'Зона','disable_insert'=>1,'pos'=>100);


$cmd['file'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['file'][CMD_BASE]->result=CMD_FILE;
$cmd['file'][CMD_BASE]->visual=Array('title'=>'Операции с файлами','group'=>GROUP_SPECIFIC,'disable_insert'=>1,'pos'=>101);


$cmd['global'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['global'][CMD_BASE]->result=CMD_GLOBAL;
$cmd['glob'][CMD_BASE]->alias=&$cmd['global'][CMD_BASE];
$cmd['global'][CMD_BASE]->visual=Array('title'=>'Глобальные операции и переменные','disable_insert'=>1,'pos'=>10);


$cmd['current_col'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cur_col, $cur_row;
		$x=prepend_type($cur_col,'cl');
		$x->row=$cur_row;
		return $x;
};
$cmd['current_col'][CMD_BASE]->result=CMD_COL;
$cmd['cucol'][CMD_BASE]->alias=&$cmd['current_col'][CMD_BASE];
$cmd['current_col'][CMD_BASE]->visual=Array('title'=>'Текущее поле объекта','condition'=>IF_CUCOL,'pos'=>0);

//Для видимости во всех частях
$cmd['col_current'][CMD_BASE]->alias=&$cmd['current_col'][CMD_BASE];
$cmd['col_current'][CMD_BASE]->visual=Array('title'=>'Текущее поле объекта','group'=>GROUP_VARS,'pos'=>10);


$cmd['pup'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $pup, $is_url_parse;
		//$next=array_shiftA($ops);
		if(empty($pup)) return false;
		if(empty($data)) $data=0;
		if($is_url_parse) $margin=1; else $margin=2;
		if(!isset($pup[count($pup)-$data-$margin])) return false;
		$cpup=$pup[count($pup)-$data-$margin];
		//if($next->data=='pup'){
		if($next=shift_first_op($ops,'pup')){
			global $cmd;
			//$x=&$cmd['pup'][CMD_BASE]->process;
			$x=get_cmd_link('pup',CMD_BASE);
			$y=$data+1;			
			return $x($y,$next,$ops);
		}
		//if($next->data=='cow'){
		if(shift_first_op($ops,'cow')){
			if(empty($cpup->cow)) return false;
			else return prepend_type($cpup->cow,'row');
		}
		//if($next->data=='table'){
		if(shift_first_op($table,'table')){
			if(empty($cpup->table)) return false;
			else return prepend_type($cpup->table,'tbl');
		}
		//if($next->data=='cex'){
		if(shift_first_op($ops,'cex')){
			if(empty($cpup->cex)) return false;
			else return prepend_type($cpup->cex,'ex');
		}
		//if($next->data=='module'){
		if(shift_first_op($ops,'module')){
			if(empty($cpup->module)) return false;
			else return prepend_type($cpup->module,'md');
		}
		//if($next->data=='part'){
		if(shift_first_op($ops,'part')){
			if(empty($cpup->part)) return false;
			else return prepend_type($cpup->part,'prt');
		}
		return false;
};
$cmd['pup'][CMD_BASE]->result=CMD_MIXED;
$cmd['pup'][CMD_BASE]->visual=Array('title'=>'Вышестоящая часть','group'=>GROUP_CONTROL,'result'=>-1,'special'=>SP_PUP,'pos'=>12);


$cmd['up'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $up,$up2,$up3,$up4,$up5,$spec_step;
		//if(empty($up)) return false;
		if(empty($spec_step)) return false;
		if(empty($data)) $data=0;				
		$x=$spec_step-$data-1;
		//if(!isset($up[$x])) return false;
		if($next=shift_first_op($ops,'up')){
			global $cmd;
			//$x=&$cmd['up'][CMD_BASE]->process;
			$x=get_cmd_link('up',CMD_BASE);
			$y=$data+1;
			return $x($y,$next,$ops);
		}
		if(shift_first_op($ops,'var') && isset($up3[$x])) return $up3[$x];
		if(shift_first_op($ops,'index') && isset($up4[$x])) return $up4[$x];
		if(shift_first_op($ops,'value') && isset($up5[$x])) return $up5[$x];
		if(isset($up[$x])){
			if(empty($up2[$x])) return $up[$x]; else return prepend_type($up[$x],$up2[$x]);
		}
};
$cmd['up'][CMD_BASE]->result=CMD_MIXED;
$cmd['up'][CMD_BASE]->visual=Array('title'=>'Вышестоящий цикл','special'=>SP_UP,'group'=>GROUP_FOR,'pos'=>13);


$cmd[''][CMD_BASE]->process=function(&$data,&$op,&$ops){
		// определение PHP функций
		global $func;
		if(!empty($op->data) && $op->data[0]=='#'){
			$php_func_name=substr($op->data,1);
			global $tpl_static,$tpl_header;
			if($php_func_name=='header'){
				$tpl_static.='[#header('.ops_to_tpl($op->put).')]';
			}
			if(in_array($php_func_name,$func) && function_exists($php_func_name)){
				if(function_exists($php_func_name.'5')) $php_func_name.='5';	// для таких функций, как например strtolower, ucfirst и т.д.			
				$arg=Array();
				check_single($op->put,$GLOBALS["lex_func_sep"]);
				if(!empty($op->put)) foreach($op->put AS $p){
					$arg[count($arg)]=parse_var($p);
				}
				if(!empty($arg)) for($i=0;$i<count($arg);$i++) {$GLOBALS['arg'.$i]=$arg[$i];}
				$GLOBALS['resu']='';
				$temp=ob_get_contents();
				if($temp) ob_end_clean();
				ob_start();
				$func2='$result='.$php_func_name.'(';
				if(!empty($arg)) for($i=0;$i<count($arg);$i++){
					if($i!=0) $func2.=',';
					$func2.='$GLOBALS["arg'.$i.'"]';
				}
				$func2.='); if(!empty($result)){$GLOBALS["resu"]=$result;}';
				$tfunc=$func2;
				eval_e5($func2);
				$tf=$func2;
				if((empty($GLOBLAS['resu']) && count($GLOBALS['resu'])==1) || is_string($GLOBALS['resu'])) $func2=ob_get_contents().$GLOBALS['resu'];
				else $func2=$GLOBALS['resu'];
				ob_end_clean();
				if($temp){
					ob_start();
					echo $temp;
				}
				if($php_func_name=='header'){
					if(empty($tpl_header)) $tpl_header=Array();
					$tpl_header[]=$GLOBALS["arg0"];
				}
				return $func2;
			}
		}
		// разбор неявных cur, cow, param
		// по идее тут бы сделать проверку по owner==0, но CMD_NONE это как бы подразумевает, так что всё норм
		if(!empty($GLOBALS['in_tree'])){	//Если речь идёт о теле цикла, то вперёд будет подставлятся cur
			// надо реализовать защиту от зацикливания
			$new_ops=$ops;
			array_unshift($new_ops,$op);
			$str='';
			if(!empty($GLOBALS["current"])){
				$c=find_cmd($op->data,CMD_ROW,$GLOBALS["current"]);
				$str=do_cmd($op,$ops,$c,$GLOBALS["current"]);
			}
			if(empty($str) && !empty($GLOBALS["cur_row"])){
				$c=find_cmd($op->data,CMD_ROW,$GLOBALS["cur_row"]);
				$str=do_cmd($op,$ops,$c,$GLOBALS["cur_row"]);
			}
			if(empty($str) && isset($GLOBALS["p_par"][$op->data])){
				//$ready_ops=$new_ops;
				//array_unshift($ready_ops, create_op('param',LEXER_CMD,'.'));
				//$str=parse_var($ready_ops);
				//if(!empty($str) && !empty($ready_ops[0])) $op=$ready_ops[0];
				global $cmd;
				array_unshift($ops,$op);
				next($ops);
				$x=&$cmd['param'][CMD_BASE]->process;
				$str=$x($data,$op,$ops);
			}
			if(empty($str) && !empty($GLOBALS["cur_ex"])){
				$c=find_cmd($op->data,CMD_EX,$GLOBALS["cur_ex"]);
				$str=do_cmd($op,$ops,$c,$GLOBALS["cur_ex"]);
			}
			if(!empty($str))  return $str;
			else return false;
		} else {
			// надо реализовать защиту от зацикливания
			$str='';
			$new_ops=$ops;
			array_unshift($new_ops,$op);
			if(!empty($GLOBALS["cur_row"])){
				$c=find_cmd($op->data,CMD_ROW,$GLOBALS["cur_row"]);
				$str=do_cmd($op,$ops,$c,$GLOBALS["cur_row"]);
			}
			if(empty($str) && isset($GLOBALS["p_par"][$op->data])){
				//$ready_ops=$new_ops;
				//array_unshift($ready_ops, create_op('param',LEXER_CMD,'.'));
				//$str=parse_var($ready_ops);
				//if(!empty($str) && !empty($ready_ops[0])) $op=$ready_ops[0];
				
				//$c=find_cmd('param',CMD_BASE,$data);
				//$c=find_cmd('param',CMD_BASE,$data);
				//array_unshift($ops,create_op('param',LEXER_CMD,'.'));
				//$str=do_cmd($op,$ops,$c,$data);				
				
				global $cmd;
				array_unshift($ops,$op);
				next($ops);
				$x=&$cmd['param'][CMD_BASE]->process;
				$str=$x($data,$op,$ops);
			}
			if(empty($str) && !empty($GLOBALS["cur_ex"]) && isset($op->data)){
				$c=find_cmd($op->data,CMD_EX,$GLOBALS["cur_ex"]);
				$str=do_cmd($op,$ops,$c,$GLOBALS["cur_ex"]);
			}
			if(!empty($str))  return $str;
			else return false;
		}
};
$cmd[''][CMD_BASE]->result=CMD_MIXED;


$cmd['param'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $p_par,$pparam,$tpl_register_globals,$db;
		
		$dop=$op->data;
		$op=array_shiftA($ops);
		if(!isset($op->data)) return false;
		$n=$op->data;
		$dst=$op->data;
		
		if(!empty($tpl_register_globals) && isset($GLOBALS[$n]) && !isset($p_par[$n])) return $GLOBALS[$n];
		
		$from_cook=0;
		if($dop=='cook') $from_cook=1;
		if($dop=='cook2') $from_cook=2;
		if($dop=='scook') $from_cook=2;
		$var2='';
		$var3=Array();		
		
		if($from_cook==0 && isset($p_par[$n])) $t_par=$p_par[$n];
		if($from_cook==1 || $from_cook==2) $GLOBALS["tpl_cookies"][$n]=1;
		if($from_cook==1 && isset($_COOKIE[$n])) $t_par=$_COOKIE[$n];
		if($from_cook==2 && isset($_COOKIE[$n])){
			$d=$_COOKIE[$n];
			if(is_array($d)){//на случай если ранее использовалось from_cook=1 с массивами
				foreach($d AS $var=>$value) SetCookie($n.'['.$var.']',"");
				$d='';
			}
			if(strstr($d,"\\\\")) $d=str_replace("\\\\",'!!-!!',$d);
			$d=str_replace("\\",'',$d);
			$d=str_replace('!!-!!',"\\\\",$d);
			$t_par=unserialize($d);
		}
		if(isset($t_par)){
			$tv2='';
			if($var2!='') $tv2=parse_var($var2);
			if(is_array($t_par)){
				if(is_object($tv2) && isset($tv2->id)) $tv2=$tv2->id;
				if($tv2=='' && is_string($tv2)) $v=$t_par;
				else if(is_object($tv2) || is_array($tv2) || !isset($t_par[$tv2])) $v='';
				else $v=$t_par[$tv2];
			} else if(isset($t_par->rows)){
				if(!isset($t_par->rows[$tv2])){
					$v=$t_par;
				} else {
					$v=$t_par->rows[$tv2];
				}
			} else {
				$v=$t_par;
			}
		} else if(!empty($n)){
			if(!empty($GLOBALS['cur_part'])){
				if(!isset($pparam[$GLOBALS['cur_part']][$n])) $pparam[$GLOBALS['cur_part']][$n]='';
				$param=$pparam[$GLOBALS['cur_part']][$n];
			}
			if($from_cook==0){
				if(!empty($param)){
					if($param['param_get'] && isset($_FILES[$n]) && isset($_FILES[$n]["name"])) $v=$_FILES[$n]["name"];
					if($param['param_get'] && isset($_POST[$n])) $v=$_POST[$n];
					if($param['param_get'] && isset($_GET[$n])) $v=urldecode($_GET[$n]);
					$v=$param['param_default'];
				} else {
					$v='';
					//if(isset($_FILES[$n]) && isset($_FILES[$n]["name"])) $v=$_FILES[$n]["name"];
					//if(isset($_POST[$n])) $v=$_POST[$n];
					//if(isset($_GET[$n])) $v=urldecode($_GET[$n]);
				}
			}
		} else return false;
		
		if(empty($ops) && $dst==$n){
			if(!empty($v->id) && !empty($v->type) && $v->type=='tbl' && strstr($v->id,':')){				
				$tmp=explode(':',$v->id);
				if(empty($v->owner)) $v->owner=0;
				if(!isset($v->tex)) $v->tex=get_tex($v->owner,$tmp[1],$tmp[0]);
				return $tmp[0].':'.$tmp[1].':'.$v->owner.'::'.$v->tex;
			}
			if(!isset($v)) $v='';
			return $v;
		}

		if(empty($ops) && $n!=$dst && !empty($v)) return $v;
		
		global $pparam;
		if(!isset($pparam[$GLOBALS['cur_part']][$n]) && !empty($GLOBALS['cur_part'])){
			getrow($db,"SELECT param_part,param_sname,param_type FROM part_param WHERE param_part=".$GLOBALS["cur_part"]." AND param_sname='$n'",1,'part_param',__LINE__,__FILE__);
			if(!empty($db->Record)) $pparam[$GLOBALS['cur_part']][$n]=$db->Record; else $pparam[$GLOBALS['cur_part']][$n]='';
		}
		if(!empty($pparam[$GLOBALS['cur_part']][$n])) $param=$pparam[$GLOBALS['cur_part']][$n];
		else $param=false;
		if(!$param){
			if(isset($v)) return $v;
			else return false;
		}
		if(empty($ops)) return $v;				
		$type=$param['param_type'];
		if($type==5) return prepend_type($v,'row');
		if($type==6){
			if(!empty($v->rows)) $pv=$v->rows; else $pv=$v;
			if(is_object($v) && shift_first_op($ops,'id')){
				$res=$v->table.':'.$v->ex.':'.$v->owner.':'.$v->stable.':'.$v->tex;
				return $res;
			} else if(shift_first_op($ops,'geturl')){
				$c=url_col($v->table);
				if(empty($c)) return '';
				else return prepend_type($c["col_id"],'cl');
			} else return $pv;
		}
		if($type==7){
			if(is_array($v)){
				$res=Array();
				foreach($v AS $av) $res[]=prepend_type($av,'cl');
				return $res;
			} else {
				return prepend_type($v,'cl');
			}
		}
		if($type==8){
			if(!empty($ops) && shift_first_op($ops,'table')){
				if(empty($ops)){
					$v=explode(':',$v);
					if(!empty($v[3])) return $v[3]; else return '';
				} else return prepend_type($v,'tbl');
			} else return prepend_type($v,'tbl');
		}

		return $v;	
};
$cmd['param'][CMD_BASE]->result=CMD_MIXED;
$cmd['cook'][CMD_BASE]->alias=&$cmd['param'][CMD_BASE];
$cmd['cook2'][CMD_BASE]->alias=&$cmd['param'][CMD_BASE];
$cmd['scook'][CMD_BASE]->alias=&$cmd['param'][CMD_BASE];
$cmd['param'][CMD_BASE]->visual=Array('title'=>'Переменная','put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>STRING_PARAM_NAME,'req'=>1)
),'special'=>SP_POINT,'pos'=>14,'convert_to_dollar'=>1);
$cmd['cook'][CMD_BASE]->visual=Array('title'=>'Переменная cookies','put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>CMD_STRING,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_VARS,'pos'=>15);
$cmd['scook'][CMD_BASE]->visual=Array('title'=>'Переменная-массив cookies','put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>CMD_STRING,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_VARS,'pos'=>16);


$cmd['ajax'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$dat=get_first_data($ops);
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,$dat,1);
};
$cmd['ajax'][CMD_BASE]->result=CMD_STRING;
//$cmd['ajax'][CMD_BASE]->visual=Array('title'=>'Ссылка для Ajax','group'=>GROUP_URL,'special'=>SP_AJAX);


$cmd['ajax2'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$dat=get_first_data($ops);
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,$dat,2);
};
$cmd['ajax2'][CMD_BASE]->result=CMD_STRING;
$cmd['ajax_min'][CMD_BASE]->alias=&$cmd['ajax'][CMD_BASE];
//$cmd['ajax_min'][CMD_BASE]->visual=Array('title'=>'Короткая ссылка для Ajax','group'=>GROUP_URL,'special'=>SP_AJAX);


$cmd['component'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,'component');
};
$cmd['component'][CMD_BASE]->result=CMD_MIXED;
$cmd['component'][CMD_BASE]->visual=Array('title'=>'Компоненты', 'special'=>SP_COMPONENT,'extend_put'=>1,'pos'=>8);


$cmd['form'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,'form');
};
$cmd['form'][CMD_BASE]->result=CMD_MIXED;
$cmd['form'][CMD_BASE]->visual=Array('title'=>'Формы', 'special'=>SP_COMPONENT,'extend_put'=>1,'pos'=>8);


$cmd['show'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,'show');
};
$cmd['show'][CMD_BASE]->result=CMD_MIXED;
$cmd['show'][CMD_BASE]->visual=Array('title'=>'Отображения', 'special'=>SP_COMPONENT,'extend_put'=>1,'pos'=>8);


$cmd['func'][CMD_BASE]->process=function(&$data,&$op,&$ops,$otype='func',$ajax=0,$pmodule=0,$cex=0,$part_name='',$part_self=0){
		global $db;
		$type=0;
		if($otype=='form')$type=3;
		if($otype=='component')$type=2;
		if($otype=='show')$type=1;
		if($otype=='function' || $otype=='func')$type=0;
		
		// Отработка для установки Ajax после вызова части или компонента - [cm.part.some_part(x=1).ajax]
		$post_skip=false;
		$next_op_data='';
		if(!empty($o_type) && !empty($ops[key($ops)+2])) $next_op_data=$ops[key($ops)+2]->data;
		if(empty($o_type) && !empty($ops[key($ops)+1])) $next_op_data=$ops[key($ops)+1]->data;
		if($next_op_data=='ajax' || $next_op_data=='ajax_min'){
			$ajax=1;
			$post_skip=true;
		}
		if($next_op_data=='ajax2'){
			$ajax=2;
			$post_skip=true;
		}
		if(empty($part_name)){
			$op1=get_first_op($ops);
			$name=$op1->data;
		} else {
			$op1=&$op;
			$name=$part_name;
		}
		if(!empty($pmodule)) $ctype=-$pmodule; else $ctype=$type;
					
		global $part_cache2;
		if(empty($part_self) && !isset($part_cache2[$ctype][$name])){
			$sql_add='';
			if(!empty($pmodule)) $sql_add=' AND part_module='.$pmodule.' AND part_type!=2';
			else $sql_add=' AND part_type=2 AND part_proc='.$type;
			getrow($db,"SELECT * FROM main_part WHERE part_sname='$name'".$sql_add,1,"main_part",__LINE__,__FILE__);
			$part_cache2[$ctype][$name]=0;
			if(!empty($db->Record)){
				$part_cache2[$ctype][$name]=$db->Record;
				$GLOBALS["part_cache4"][$db->Record["part_id"]]=$db->Record;
			}
		}
		if(!empty($part_cache2[$ctype][$name]) || !empty($part_self)){
			$bg=backup_globals();
			if(!empty($part_self)) $db->Record=$part_self;
			else $db->Record=$part_cache2[$ctype][$name];
			global $puid,$p_par, $url_row2;
			$part_id=$db->Record['part_id'];
			$part_parse=$db->Record['part_parse'];
			$part_major=$db->Record['part_major'];
			$part_body=$db->Record['part_body'];
			$cur_ex=$GLOBALS['cur_ex'];
			check_single($op1->put,$GLOBALS["lex_func_sep"]);
			$clear_vars=($pmodule==0 || $ajax==1/*2*/);
			$par=prepend_param($op1->put,$part_major,$part_id,$clear_vars);
			/*if($part_id==93 || $part_id==95){
				var_dump($p_par);
				echo '<br><br>';
			}*/
			if(!empty($pmodule)){
				$module=$pmodule;
				global $modname_cache;
				if(!isset($modname_cache[$module])) $modname_cache[$module]=getrowval("SELECT module_id, module_sname FROM main_module WHERE module_id=$module","module_sname");
				$modname=$modname_cache[$module];
				if($module!=$GLOBALS["cur_module"]  && !empty($url_row2[$module])){
					$GLOBALS["cur_row"]=$url_row2[$module];
				}
				$cur_module=$module; if($cex!=0) $cur_ex=$cex; else $cur_ex=$GLOBALS["cur_ex"];
				if($ajax!=0){
					$p_par["cur_ex"]=$cur_ex;
					$p_par["cur_row"]=$GLOBALS["cur_row"];
				}
				if($ajax!=0) $p_par['ajax']=$modname.'.'.$name;
			} else {
				if($ajax!=0) $p_par['ajax']=$otype.'.'.$name;
			}
			if($ajax==0){
				$r=shell_part($part_id,$cur_ex,$GLOBALS['cur_row']);
			}
			else {
				$rurl=$GLOBALS['prot'].'://'.$_SERVER['HTTP_HOST'];
				$rurl2=$_SERVER['REQUEST_URI'];
				if($ajax/*!=2*/!=1) $rurl=concat_with_zone($rurl,$rurl2);	
				$r=convert_url($rurl,0,$p_par);
			}
			$p_par=$par;
			//if($part_id==93 || $part_id==95) var_dump($GLOBALS['p_par']['block_tpl']);
			return_globals($bg);			
			if($post_skip) $op=array_shiftA($ops);
			return $r;
		} else {
			if($post_skip) $op=array_shiftA($ops);
			return '';
		}
};
$cmd['func'][CMD_BASE]->result=CMD_MIXED;
$cmd['function'][CMD_BASE]->alias=&$cmd['func'][CMD_BASE];
$cmd['func'][CMD_BASE]->visual=Array('title'=>'Функции', 'special'=>SP_COMPONENT,'extend_put'=>1,'pos'=>8);


$cmd['user'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $user;
		return $user->id;
};
$cmd['user'][CMD_BASE]->result=CMD_USER;
$cmd['user'][CMD_BASE]->visual=Array('title'=>'Команды пользователя','disable_insert'=>1,'is_cur_user'=>1,'pos'=>12);


$cmd['group'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $db;
		$op=array_shiftA($ops);
		$name=$op->data;
		$group_id=getrowval("SELECT * FROM main_auth WHERE group_sname='$name'","auth_id");
		return prepend_type($group_id,'grp');
};
$cmd['group'][CMD_BASE]->result=CMD_GROUP;
$cmd['group'][CMD_BASE]->visual=Array('title'=>'Группы пользователей','put'=>Array(
		0=>Array('title'=>'группа','type'=>STRING_GROUP_NAME,'req'=>1)
),'special'=>SP_POINT,'pos'=>12);


$cmd['any_group'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return prepend_type(-2,'grp');
};
$cmd['any_group'][CMD_BASE]->result=CMD_GROUP;
$cmd['any_group'][CMD_BASE]->visual=Array('title'=>'Неизвестная группа пользователей','pos'=>100,'group'=>GROUP_SPECIFIC);


$cmd['include'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $allow_components;
		$type=array_shiftA($ops);
		$type=$type->data;
		$name=$ops[key($ops)]->data;
		if(empty($allow_components[$type])) return false;
		$paths=Array();
		if($type=='editor'){
			$paths[DOCUMENT_ROOT.'/core/editor/components/']=1;
			$paths[DOCUMENT_ROOT.'/core/editor/tpl/']=1;
		}
		if(empty($paths)) return false;
		$tops=dc($ops);
		$first=true;
		foreach($paths AS $path=>$tmp){
			if(!$first) $ops=dc($tops);
			$first=false;
			while(count($ops)>0){
				//echo $path.$name.'.inc'.'<br>';
				if(file_exists($path.$name.'.inc')){
					include_once($path.$name.'.inc');
					return prepend_type('','component');
				} else if(file_exists($path.$name.'.tpl')){
					global $p_par;
					if(!empty($ops[key($ops)]->put)){
						check_single($ops[key($ops)]->put,$GLOBALS["lex_func_sep"]);
						$par=prepend_param($ops[key($ops)]->put);
					}
					$tmp=shell_tpl(file_get_contents($path.$name.'.tpl'));
					if(isset($par)) $p_par=$par;
					array_shiftA($ops);
					return $tmp;
				} else if(file_exists($path.$name) && is_dir($path.$name)){
					$path.=$name.'/';
					$tmp=array_shiftA($ops);
					$name=$ops[key($ops)]->data;
				} else break;
			}
		}
};
$cmd['include'][CMD_BASE]->result=CMD_MIXED;


$cmd['block'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $blocks_data, $p_par;
		$op=array_shiftA($ops);
		$name=$op->data;
		if(!isset($blocks_data[$name])) return false;
		if(!empty($op->put)){
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			$par=prepend_param($op->put);
		}
		$result=shell_tpl($blocks_data[$name]);
		if(isset($par)) $p_par=$par;
		return $result;
};
$cmd['block'][CMD_BASE]->result=CMD_MIXED;
$cmd['block'][CMD_BASE]->visual=Array('title'=>'Вывести блок','put'=>Array(
		0=>Array('title'=>'Название','type'=>STRING_BLOCK_NAME,'req'=>1)
),'special'=>SP_POINT,'extend_put'=>1,'group'=>GROUP_CONTROL,'pos'=>30);


$cmd['lng'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return lng(parse_var($op->put));
};
$cmd['lng'][CMD_BASE]->result=CMD_STRING;


$cmd['icon'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$name=parse_var($op->put[0]);
		if(isset($op->put[1])) $hspace=parse_var($op->put[1]); else $hspace=5;
		if(isset($op->put[2])) $vspace=parse_var($op->put[2]); else $vspace=0;
		if(isset($op->put[3])) $title=parse_var($op->put[3]); else $title='';
		return si($name,$hspace,$vspace,$title);
};
$cmd['icon'][CMD_BASE]->result=CMD_STRING;


$cmd['button'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$name=parse_var($op->put[0]);
		if(isset($op->put[1])) $url=parse_var($op->put[1]); else $url='';
		if(isset($op->put[2])) $title=parse_var($op->put[2]); else $title='';
		if(isset($op->put[3])) $addon=parse_var($op->put[3]); else $addon='';
		if(isset($op->put[4])) $is_url=parse_var($op->put[4]); else $is_url=1;
		if(isset($op->put[5])) $hspace=parse_var($op->put[5]); else $hspace=5;
		if(isset($op->put[6])) $vspace=parse_var($op->put[6]); else $vspace=0;
		if(isset($op->put[7])) $addon2=parse_var($op->put[7]); else $addon2='';
		if(isset($op->put[8])) $addon3=parse_var($op->put[8]); else $addon3='';
		return se($name,$url,$title,$addon,$is_url,$hspace,$vspace,$addon2,$addon3);
};
$cmd['button'][CMD_BASE]->result=CMD_STRING;


$cmd['widget'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $db,$cmd;
		$op=array_shiftA($ops);
		$module=$op->data;
		$op=array_shiftA($ops);
		$ex=$op->data;
		$op=array_shiftA($ops);
		$part=$op->data;
		$module=getrowval("SELECT module_id FROM main_module WHERE module_sname='".$module."'",'module_id');
		if(!$module/* || !check_mod($module,'view')*/) return false;/* явно, что у клиента этих доступов может и не быть */
		$ex=getrowval("SELECT ex_id FROM ex_module WHERE ex_sname='".$ex."' AND ex_module=".$module,'ex_id');
		if(!$ex/* || !check_ex($ex,'view')*/) return false;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,'',0,$module,$ex,$part);		
};
$cmd['widget'][CMD_BASE]->result=CMD_MIXED;
$cmd['widget'][CMD_BASE]->visual=Array('title'=>'Виджеты модулей','special'=>SP_WIDGET,'pos'=>9,'condition'=>IF_WIDGET,'disable_insert'=>1);


$cmd['helpdesk'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['helpdesk'][CMD_BASE]->result=CMD_MIXED;
$cmd['helpdesk'][CMD_BASE]->visual=Array('title'=>'Справочник','special'=>SP_IGNORE,'pos'=>100,'disable_insert'=>1);


$cmd['cache'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['cache'][CMD_BASE]->result=CMD_NONE;
$cmd['cache'][CMD_BASE]->visual=Array('title'=>'Настройки кеширования (вставлять в начале шаблона)','special'=>SP_CACHE,'group'=>GROUP_CONTROL, 'put'=>Array(
		0=>Array('title'=>'Привязка','type'=>CMD_STATIC,'list'=>Array(
			'&'=>'К экземпляру, URL, переменным окружению, значению циклов и cookies',
			'*'=>'К экземпляру модуля',
			'#'=>'К экземпляру модуля, переменным окружения и cookies',
			'%'=>'К экземпляру модуля, переменным окружения, cookies и значению циклов',
			'_'=>'К значению циклов'
		),'req'=>1),
		1=>Array('title'=>'Не привязывать к авторизации','type'=>CMD_STATIC,'static'=>'~'),
		2=>Array('title'=>'Отключить автоудаление','type'=>CMD_STATIC,'static'=>'@'),
		3=>Array('title'=>'Привязать к текущему host','type'=>CMD_STATIC,'static'=>'!'),
		4=>Array('title'=>'Время жизни','type'=>CMD_STRING),
		5=>Array('title'=>'Еденицы времени жизни','type'=>CMD_STATIC,'list'=>Array(''=>'В минутах', '$'=>'В днях'))
));


$cmd['use'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['use'][CMD_BASE]->result=CMD_NONE;
$cmd['use'][CMD_BASE]->visual=Array('title'=>'Использовать шаблон','group'=>GROUP_CONTROL,'special'=>SP_SPACE,'put'=>Array(
		0=>Array('title'=>'Тип','type'=>CMD_STATIC,'list'=>Array(
			'module'=>"Часть текущего модуля",
			'up'=>'Вышестоящая часть',
			'function'=>'Функция',
			'show'=>'Отображение',
			'component'=>'Компонент',
			'form'=>'Форма',
			'tpl'=>'Свой шаблон'
		),'req'=>1),
		1=>Array('title'=>'Источник (имя компонента/части или шаблон)','type'=>CMD_STRING)
));


$cmd['timer'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		$op=array_shiftA($ops);
		$name='';
		if(!empty($op->put)) $name=parse_var($op->put);
		if($op->data=='start'){
			if(!empty($name)) $GLOBALS['timers'][$name]=start_timer();
			else start_timer();
			return false;
		} else {
			if(!empty($name)) return end_timer($GLOBALS['timers'][$name]);
			else return end_timer();
		}
};
$cmd['timer'][CMD_BASE]->result=CMD_STRING;
$cmd['timer'][CMD_BASE]->visual=Array('title'=>'Таймер отладки','group'=>GROUP_SPECIFIC,'special'=>SP_POINT,'put'=>Array(
		0=>Array('title'=>'Действие','list'=>Array('start'=>'Запустить','stop'=>'Остановить'),'req'=>1),
		1=>Array('title'=>'Имя','type'=>CMD_STRING)
));


$cmd['part_param'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		return false;
};
$cmd['part_param'][CMD_BASE]->result=CMD_MIXED;
$cmd['part_param'][CMD_BASE]->visual=Array('title'=>'Переменные части',/*'pos'=>1,*/'disable_insert'=>1,'special'=>SP_PART_PARAM,'skip_folder'=>1,'condition'=>IF_PART_HAVE_PARAM,'group'=>GROUP_MAIN_PART,'pos'=>2);


$cmd['cpart'][CMD_BASE]->process=function(&$data,&$op,&$ops){
		global $cur_part;
		return $cur_part;
};
$cmd['cpart'][CMD_BASE]->result=CMD_PART;
$cmd['cpart'][CMD_BASE]->visual=Array('title'=>'Текущая исполняемая часть','group'=>GROUP_MAIN_PART,'pos'=>1);


?>