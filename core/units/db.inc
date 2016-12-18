<?php

function remove_db($database,&$db=0){
	if(empty($db)) global $db;
	$tbls=getall($db,"SHOW TABLES FROM `".$database."`");	if(!empty($tbls)) foreach($tbls AS $t) $db->query("DROP TABLE `".array_shift($t)."`");
}

function table_exists($table_name){
	global $db;
	$b=false; $tbls=getall($db,"SHOW TABLES FROM `".$GLOBALS["database"]."`"); if(!empty($tbls)) foreach($tbls AS $t) if(array_shift($t)==$table_name){$b=true; break;}
	return $b;
}

function load_dump($dump,&$db,$remove='',$dfile=''){
	//global $db;
	if(!empty($remove)) remove_db($remove,$db);
	if(empty($dfile)){
		$dump=explode(';',$dump);foreach($dump AS $d) $db->fast_query($d);
	} else {
		$vs=';
';
		$cmd='';
		$f=fopen($dfile,'r');
		while(!feof($f)){
			$cmd.=fread($f,1024*/*1024*/256);
			if(strpos($cmd,$vs)!==false){
				$sql=explode($vs,$cmd);
				for($i=0;$i<count($sql)-1;$i++){
					if(!empty($sql[$i])) $db->fast_query($sql[$i]);
				}
				$cmd=$sql[count($sql)-1];
			}
		}
		if($cmd!='') $db->fast_query($cmd);
		fclose($f);
	}
}

function true_unicode($db=0){
	global $correct_utf;
	if(!is_object($db) && $db==0) $db=$GLOBALS["db"];
	if(!$db){
		$db->query("INSERT INTO row_value (value_value,value_module) VALUES ('Иш',-10)");getrow($db,"SELECT * FROM row_value WHERE value_module=-10");
		if($db->Record["value_value"]!='Иш') $correct_utf=1; else $correct_utf=0;$db->query("DELETE FROM row_value WHERE value_module=-10");
		save_config();
	}
}

function getrow(&$dbh,$query,$type=2,$table='',$line=0,$file='') {
	$dbh->query($query,$type,$table,$line,$file);
	$dbh->next_record();
	$dbh->free();
	return $dbh->Record;
}

function getrowval($query,$col_sname){
	global $db;
	getrow($db,$query);
	if(!empty($db->Record) && isset($db->Record[$col_sname])) return $db->Record[$col_sname]; else return false;
}

function getrow2($query,$col_sname){
	return getrowval($query,$col_sname);
}

function getAll2(&$dbh,$query,$res=Array(),$type=2,$table='',$line=0,$file=''){
	$dbh->query($query,$type,$table,$line,$file);
	$i=count($res);
	while($dbh->next_record()){
		$res[$i]=$dbh->Record;
		$i++;
	}
	$dbh->free();//this part deleted
	return $res;
}

function getall3(&$dbh,$query,$field,$res=Array(),$skip_empty=false){		//сохраняет элементы в массив array[row[field]]=row[field]  (array[field_value]=field_value)
	$tmp=getall($dbh,$query);
	if($skip_empty){
		if(!empty($tmp)) foreach($tmp AS $tm){
			if(isset($tm[$field]) && !empty($tm[$field])) $res[$tm[$field]]=$tm[$field];
		}
	} else {
		if(!empty($tmp)) foreach($tmp AS $tm){
			if(isset($tm[$field])) $res[$tm[$field]]=$tm[$field];
		}
	}
	return $res;
}

function getall4(&$dbh,$query,$field,$res=Array()){		//выходной массив имеет формат array[field_value]=row
	$tmp=getall($dbh,$query);
	if(!empty($tmp)) foreach($tmp AS $tm) $res[$tm[$field]]=$tm;
	return $res;
}

function getall5(&$dbh,$query,$field,$res=Array()){		//выходной массив имеет формат array[field][i]=row
	$tmp=getall($dbh,$query);
	$res=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) $res[$tm[$field]][]=$tm;
	return $res;
}

function getall6(&$dbh,$query,$field,$field2,$res=Array()){		//выходной массив имеет формат array[field]=row[field2]
	$tmp=getall($dbh,$query);
	if(!empty($tmp)) foreach($tmp AS $tm) $res[$tm[$field]]=$tm[$field2];
	return $res;
}

function getall7(&$dbh,$query,$field,$res=Array()){		//сохраняет элементы в массив array[i]=row[field]  (array[i]=field_value)
	$tmp=getall($dbh,$query);
	$i=0;
	if(!empty($tmp)) foreach($tmp AS $tm){		
		if(isset($tm[$field])){
			$res[$i]=$tm[$field];
			$i++;
		}
	}
	return $res;
}

function getall8(&$dbh,$query,$field,$field2,$res=Array()){		//выходной массив имеет формат array[field][i]=row[field2]
	$tmp=getall($dbh,$query);
	if(!empty($tmp)) foreach($tmp AS $tm) $res[$tm[$field]][]=$tm[$field2];
	return $res;
}

function getall9(&$dbh,$query,$field,$field2,$res=Array()){		//выходной массив имеет формат array[field][field2]=row[field2]
	$tmp=getall($dbh,$query);
	if(!empty($tmp)) foreach($tmp AS $tm) $res[$tm[$field]][$tm[$field2]]=$tm[$field2];
	return $res;
}

function getall10(&$dbh,$query,$field,$field2,$res=Array()){		//выходной массив имеет формат array[field][field2]=row
	$tmp=getall($dbh,$query);
	if(!empty($tmp)) foreach($tmp AS $tm) $res[$tm[$field]][$tm[$field2]]=$tm;
	return $res;
}

function backup_db($filename,$limit=30000,$except_tables=Array(),$except_tables_data=Array(),$use_static_header=1,$use_gzip=1){//лимит отвечает за максимальное символов в одной SQL секции
	global $db,$database;
	
	if(!empty($except_tables)) $except_tables=array_flip($except_tables);
	if(!empty($except_tables_data)) $except_tables_data=array_flip($except_tables_data);
	
	if(file_exists($filename)) unlink($filename);
	if($use_gzip) $f=gzopen($filename,'wb9');
	else $f=fopen($filename,'w');
	
	$tbls=getall($db,"SHOW TABLES FROM `".$database."`");
	if(!$use_static_header){
		$nl=VSP2.'   ';
		$lines='';
		if(!empty($tbls)) foreach($tbls AS $tbl){
			$tbl_name=array_shift($tbl);
			if(!empty($except_tables[$tbl_name])) continue;
			$cols=getall($db,"SHOW COLUMNS FROM `".$tbl_name."`");
			$cols_cache[$tbl_name]=$cols;
			$line='CREATE TABLE `'.$tbl_name.'` (';
			$pri=Array();
			$key=Array();
			$lineA=Array();
			foreach($cols AS $col){
				$is_text=false;
				$is_date=false;
				$val=Array();
				foreach($col AS $var=>$value){
					if($var=='Type' && $value=='text') $is_text=true;
					if($var=='Type' && $value=='longtext') $is_text=true;
					if($var=='Type' && $value=='tinytext') $is_text=true;
					if($var=='Type' && $value=='date') $is_date=true;
					if($var=='Type' && $value=='time') $is_date=true;
					if($var=='Type' && $value=='datetime') $is_date=true;
					if($var=='Type' && $value=='timestamp') $is_date=true;
					if(($is_text || $is_date) && $var=='Default' && $value!='') $value="'".$value."'";
					if(!$is_text && $var=='Null' && $value=='NO') $value='NOT NULL';
					if($is_text && $var=='Null' && $value=='NO') $value='NOT NULL';
					if($is_text && $var=='Null' && $value=='YES') $value='NULL';
					if(!$is_text && $value=='YES') $value='';
					$val[$var]=$value;
				}
				$lineB=$nl.'`'.$val["Field"].'` '.$val["Type"].' ';
				if(!empty($val["Null"])) $lineB.=$val["Null"].' ';
				if(isset($val["Default"]) && $val["Default"]!=''){
					$lineB.="DEFAULT ".$val["Default"];
				}
				if(!empty($val["Extra"])) $lineB.=' '.$val["Extra"];			
				if(!empty($val["Key"]) && $val["Key"]=='PRI') $pri[$val["Field"]]=$val["Field"];
				if(!empty($val["Key"]) && $val["Key"]!='PRI') $key[$val["Field"]]=$val["Field"];
				$lineA[]=$lineB;
			}
			foreach($pri AS $val) $lineA[]=$nl.'PRIMARY KEY (`'.$val.'`)';
			foreach($key AS $val) $lineA[]=$nl.'KEY `'.$val.'` (`'.$val.'`)';
			$line.=implode(',',$lineA);
			$line.=VSP2.') ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;';
			$line.=VSP2.VSP2;
			$lines.=$line;
		}
	} else {
		$lines=file_get_contents(DOCUMENT_ROOT.'/core/install/dump.sql');
	}
	
	if($use_gzip) gzwrite($f,$lines);
	else fwrite($f,$lines);
	
	$first3=true;
	$lts='';
	if(!empty($tbls)) foreach($tbls AS $tbl){		
		$tbl_name=array_shift($tbl);
		if(!empty($except_tables_data[$tbl_name])) continue;
		if(empty($cols_cache[$tbl_name])) $cols_cache[$tbl_name]=getall($db,"SHOW COLUMNS FROM `".$tbl_name."`");
		getrow($db,"SELECT * FROM ".$tbl_name);
		$ts='';
		if(!empty($db->Record)){
			$cols=$cols_cache[$tbl_name];
			if(!$first3) $ts.=';';
			$first3=false;
			$ds='

INSERT INTO `'.$tbl_name.'` (';
			$cc=Array();
			$first=true;
			$i=0;
			foreach($cols AS $col){
				if(!$first) $ds.=',';
				$first=false;
				$is_text=false;
				$is_date=false;
				$name='';
				foreach($col AS $var=>$value){
					if($var=='Field') $name=$value;
					if($var=='Type'){
						$value2=$value;
						if(strstr($value2,'(')){
							$value2=explode('(',$value2);
							$value2=$value2[0];
						}
					}
					if($var=='Type' && $value2=='text') $is_text=true;
					if($var=='Type' && $value2=='tinytext') $is_text=true;					
					if($var=='Type' && $value2=='date') $is_date=true;
					if($var=='Type' && $value2=='time') $is_date=true;			
					if($var=='Type' && $value2=='datetime') $is_date=true;
					if($var=='Type' && $value2=='timestamp') $is_date=true;
					if($var=='Type' && $value2=='longtext') $is_text=true;
					if($var=='Type' && $value2=='varchar') $is_text=true;
				}
				if($is_text || $is_date) $cc[$i][$name]=1; else $cc[$i][$name]=0;
				$ds.=$name;
				$i++;
			}
			$ds.=') VALUES';
			$ts.=$ds;
			$tl=0;
			$first2=true;
			$db->query("SELECT * FROM ".$tbl_name);
			$next_time_add_ds=false;
			while($db->next_record()){
			//foreach($vals AS $val){
				if($next_time_add_ds) $ts.=';'.$ds;
				$next_time_add_ds=false;
				$val=$db->Record;
				if(!$first2) $ts.=',';
				$first2=false;
				$zs='
(';
				$first=true;
				foreach($cc AS $index=>$c) foreach($c AS $name=>$quotes){
					if(!$first) $zs.=',';
					$first=false;
					if(isset($val[$name])) $v=$val[$name]; else $v='';
					$v=safe_sql_input($v);
					//$v=addslashes($v);		
					if(!$quotes && $v==='') $v='0';
					if ($quotes) $zs.="'";
					$zs.=$v;
					if ($quotes) $zs.="'";
				}
				$zs.=')';
				$tl+=strlen($zs);
				$ts.=$zs;
				if($tl>$limit){
					$first2=true;
					if($use_gzip) gzwrite($f,$ts);
					else fwrite($f,$ts);
					$ts='';
					$tl=0;
					$next_time_add_ds=true;
				}
			}
		}
		//$t.=$ts;
		if($use_gzip) gzwrite($f,$ts);
		else fwrite($f,$ts);
		$lts=$ts;
	}
	
	if($lts[strlen($lts)-1]==')'){
		if($use_gzip) gzwrite($f,';');
		else fwrite($f,';');
	}
	if($use_gzip) gzclose($f); else fclose($f);
}

function getAll(&$dbh,$query,$type=2,$table='',$line=0,$file=''){
	$dbh->query($query,$type,$table,$line,$file);
	$result=Array();
	while($dbh->next_record()) $result[]=$dbh->Record;
	$dbh->free();
	return $result;
}

class DB_Sql {
  var $Host     = "";
  var $Database = "";
  var $User     = "";
  var $Password = "";
  var $Auto_Free     = 0;
  var $Debug         = 0;
  var $Halt_On_Error = "yes";
  var $PConnect      = 0;
  var $Seq_Table     = "db_sequence";
  var $Record   = array();
  var $Records   = array();
  var $Row;
  var $type = 2;
  var $BaseName;
  var $Errno    = 0;
  var $Error    = "";
  var $dtype   = "mysqli";
  var $Link_ID  = 0;
  var $Query_ID = 0;
  var $locked   = false;
  var $port="";
  var $error=false;
function connect() {
	if ( !$this->Link_ID ) {
		if($this->dtype=='mysql'){
			@$this->Link_ID = mysql_connect($this->Host, $this->User, $this->Password);			
			if ($this->Link_ID && !@mysql_select_db($this->Database,$this->Link_ID)) {
				error_reporting(E_NOTICE);
				ini_set('display_errors', 0); 
				$this->halt("connect($this->Database) failed.");
				return 0;
			}
		}
		if($this->dtype=='mysqli'){
			@$this->Link_ID=new mysqli($this->Host, $this->User, $this->Password, $this->Database);
		}
		if($this->dtype=='postgre'){
			if(!empty($port)) $pname=" port=$port"; else $pname='';
			@$this->Link_ID=pg_connect("host=$this->Host".$pname." options='--client_encoding=UTF8' dbname=$this->Database user=$this->User password=$this->Password");
		}
		if (!$this->Link_ID) {
			$this->halt("connect($this->Host, $this->User, \$this->Password) failed.");
			return 0;
		}
	}
	unset($this->User);
	unset($this->Password);
	return $this->Link_ID;
}

function last_insert_id(){
	if(is_object($this->Link_ID) && isset($this->Link_ID->insert_id)) return $this->Link_ID->insert_id;
	else {		
		if($this->dtype=='mysql') return mysql_insert_id($this->Link_ID);
		if($this->dtype=='mysqli') return mysqli_insert_id($this->Link_ID);
	}
}

function free() {
	//global $freesq;
	//if(!$GLOBALS["cache_sql2"] && !empty($this->Query_ID) && empty($freesq[$this->Query_ID]) && $this->Query_ID)
	if(!$GLOBALS["cache_sql2"]){
		if($this->dtype=='mysql' && is_resource($this->Query_ID)) mysql_free_result($this->Query_ID);
		if($this->dtype=='mysqli' && is_object($this->Query_ID)) mysqli_free_result($this->Query_ID);
		if($this->dtype=='postgre' && is_resource($this->Query_ID)) pg_free_result($this->Query_ID);
	}
	//$freesq[$this->Query_ID]=1;
	$this->Query_ID = 0;
}

function escape($str){
	if($this->dtype=='mysql') return mysql_real_escape_string($str);
	if($this->dtype=='mysqli') return mysqli_real_escape_string($this->Link_ID,$str);
}

function next_record() {
	if($this->type==2){
		if (!$this->Query_ID) {
			$this->halt("next_record called with no query pending.");
			return 0;
		}
		if($GLOBALS["cache_sql2"]){
			if($this->dtype=='mysql') $nr=mysql_num_rows($this->Query_ID);
			if($this->dtype=='mysqli'){
				if(is_object($this->Query_ID)) $nr=$this->Query_ID->num_rows;
				else $nr=0;
			}
			if($this->dtype=='postgre') $nr=pg_num_rows($this->Query_ID);
			if($nr>$this->Row){
				if($this->dtype=='mysql'){
					mysql_data_seek($this->Query_ID,$this->Row);
					$this->Record = mysql_fetch_array($this->Query_ID,MYSQL_ASSOC);
					$this->Errno  = mysql_errno();
					$this->Error  = mysql_error();
				}
				if($this->dtype=='mysqli'){
					mysqli_data_seek($this->Query_ID,$this->Row);
					$this->Record = $this->Query_ID->fetch_array(MYSQLI_ASSOC);
					$this->Errno  = $this->Link_ID->errno;
					$this->Error  = $this->Link_ID->error;
				}
				if($this->dtype=='postgre'){
					pg_result_seek($this->Query_ID,$this->Row);
					$this->Record = pg_fetch_array($this->Query_ID, NULL, PGSQL_ASSOC);
					$this->Errno  = pg_result_status($this->Query_ID);
					$this->Error  = pg_last_error($this->Link_ID);
				}
				$this->Row   += 1;
				$stat = is_array($this->Record);
				if (!$stat && $this->Auto_Free) {
					$this->free();
				}
				return $stat;
			} else {
				$this->Record=false;
				$this->Row   += 1;
				return false;
			}
		} else {
			if(is_resource($this->Query_ID) || is_object($this->Query_ID)){
				if($this->dtype=='mysql'){
					$this->Record = mysql_fetch_array($this->Query_ID,MYSQL_ASSOC);
					$this->Errno  = mysql_errno();
					$this->Error  = mysql_error();
				}
				if($this->dtype=='mysqli'){
					$this->Record = $this->Query_ID->fetch_array(MYSQLI_ASSOC);
					$this->Errno  = $this->Link_ID->errno;
					$this->Error  = $this->Link_ID->error;
				}
				if($this->dtype=='postgre'){
					$this->Record = pg_fetch_array($this->Query_ID, NULL, PGSQL_ASSOC);
					$this->Errno  = pg_result_status($this->Query_ID);
					$this->Error  = pg_last_error($this->Link_ID);
				}
				
				$this->Row   += 1;
				$stat = is_array($this->Record);
				if (!$stat && $this->Auto_Free) {
					$this->free();
				}		
				return $stat;
			} else {
				$this->Record=false;
				$this->Row   += 1;
				return false;
			}
		}
	} else {
		if(isset($this->Records[$this->Row])) $this->Record = $this->Records[$this->Row];
		else $this->Record=Array();
		$this->Row+=1;
		return !empty($this->Record);
	}
}

function num_rows(){
	if($this->type!=2){
		if(empty($this->Records)) return false;
		return count($this->Records);
	}else {
		if($this->dtype=='mysql') return mysql_num_rows($this->Query_ID);
		if($this->dtype=='mysqli'){
			if(is_object($this->Query_ID)) return $this->Query_ID->num_rows;
			else return 0;
		}
		if($this->dtype=='postgre') return pg_num_rows($this->Query_ID);
	}
}

function halt($msg) {
	if($this->dtype=='mysql'){
		$this->Error = @mysql_error($this->Link_ID);
		$this->Errno = @mysql_errno($this->Link_ID);
	}
	if($this->dtype=='mysqli'){
		$this->Error = $this->Link_ID->error;
		$this->Errno = $this->Link_ID->errno;
	}
	if($this->dtype=='postgre'){
		$this->Errno  = pg_connection_status($this->Link_ID);
		$this->Error  = pg_last_error($this->Link_ID);
	}
	if((strstr($this->Error,'Got error 127 from storage engine') || strstr(strtolower($this->Error),'repair')) && !empty($GLOBALS["lsql"]) && strstr($GLOBALS["lsql"],"FROM") && strstr($GLOBALS["lsql"],"WHERE")){
		$tname=get_tag($GLOBALS["lsql"],"FROM "," WHERE");
		if($this->dtype=='mysql') @mysql_query("REPAIR TABLE `".$tname."`",$this->Link_ID);
		if($this->dtype=='mysqli') @$this->Link_ID->query("REPAIR TABLE `".$tname."`");
		if($this->dtype=='postgre') @pg_query($this->Link_ID,"REPAIR TABLE `".$tname."`");
	}
	if ($this->Halt_On_Error == "no"){ echo 'Error'; $this->error=true; return '';}
	if ($this->Halt_On_Error != "report") fire('db-error',$msg. '<br>(Error - '.$this->Error.'; Errno - '.$this->Errno.')');
}

function fast_query($qs){
	$GLOBALS["lsql"]=$qs;
	if($this->dtype=='mysql'){
		$this->Query_ID = mysql_query($qs,$this->Link_ID);
		$this->Errno = mysql_errno();
		$this->Error = mysql_error();
	}
	if($this->dtype=='mysqli'){
		$this->Query_ID = $this->Link_ID->query($qs);
		$this->Error = $this->Link_ID->error;
		$this->Errno = $this->Link_ID->errno;
	}
	if($this->dtype=='postgre'){
		$this->Query_ID = pg_query($this->Link_ID,$qs);
		$this->Errno  = pg_result_status($this->Query_ID);
		$this->Error  = pg_last_error($this->Link_ID);
	}
	if (!$this->Query_ID) {
		$this->halt("Invalid SQL: ".$qs);
	}
	return $this->Query_ID;
}

function query($qs,$type=2,$table='',$line=0,$file='') {
	global $tquery,$aquery,$tqrow,$qc,$debug,$dnct,$use_dnct,$cache_sql2,$do_sql_log;
	$otype=$type;
	if(!isset($GLOBALS["iqs"])) $GLOBALS["iqs"]=false;
	if(!isset($GLOBALS["cache_sql"])) $GLOBALS["cache_sql"]=false;
	if($GLOBALS["cache_sql"] && empty($table)){
		$x=strtolower(substr($qs,0,6));
		if($x=='select'){
			$table=get_tag($qs.' ',' FROM ',' ');
			if($type==2 && !empty($table)){
				$type=1;
				$otype=1;
			}
		} else {
			if($x=='delete' || $x=='update' || $x=='insert'){
				if($x=='insert') $table=get_tag($qs.' ','INSERT INTO ',' ');
				if($x=='delete') $table=get_tag($qs.' ',' FROM ',' ');
				if($x=='update') $table=get_tag($qs.' ','UPDATE ',' ');
				$type=3;
				$otype=3;
			}
		}
	}
	if($GLOBALS["iqs"]) stf('query');
	if(!$GLOBALS["cache_sql"]) $type=2;	
	if(!empty($dnct[$table]) && $use_dnct) $type=2;
	$GLOBALS["lsql"]=$qs;	
	$this->type=$type;
	$this->Row   = 0;
	if($do_sql_log){
		if(!file_exists(DOCUMENT_ROOT.'/core/log')){
			mkdir(DOCUMENT_ROOT.'/core/log');
			chmod(DOCUMENT_ROOT.'/core/log',DEF_DRMOD);
		}
		$filename=DOCUMENT_ROOT.'/core/log/sql.log';
		$f=fopen($filename,'a');
		fwrite($f,VSP.date('Y-m-d H:i:s').' / '.$_SERVER["REQUEST_URI"].' / '.$qs.' / '.$line.' / '.$file);
		fclose($f);
	}
	if($debug){
		$mtime2 = microtime();$mtime2 = explode(" ",$mtime2);$mtime2 = $mtime2[1] + $mtime2[0];
	}
	if($type!=2){
		//$qss=urlencode($qs);
		//$qss=crc32($qs);
		//$qss=sha1($qs);
		$qss=md5($qs);
		if($type==3){
			if(file_exists(FTEMP.$table)){
				removeDirRec(FTEMP.$table);
			}
			unset($qc[$table]);
		}
		if($type==1 && file_exists(FTEMP.$table.'/'.$qss) && !isset($qc[$table][$qss])){
			
			/*$tmp=file_get_contents(FTEMP.$table.'/'.$qss);
			$cl=strlen($tmp);
			$i=0;$s='';$b='';$t=0;$v=Array();$x='';
			$z1=SEP1;$z2=SEP2;$z3=SEP3;
			$x1=$z1[0];$x2=$z1[1];
			$z1=$z1[2];$z2=$z2[2];$z3=$z3[2];			
			if($cl>0) for($z=0;$z<$cl;$z++){
				$j=$tmp[$z];
				if($j!=$x1 || $tmp[$z+1]!=$x2){
					if($t==0) $s.=$j;
					else $b.=$j;
				} else {
					$j=$tmp[$z+2];
					if($j==$z1){
						if($s!='' && $t==1) $v[$i][$s]=$b;
						$s='';$t=0;$b='';$i++;$z+=2; continue;
					} else if($j==$z2){
						if($s!='' && $t==1) $v[$i][$s]=$b;
						$s='';$t=0;$b='';$z+=2; continue;
					} else if($j==$z3){ $t=1;$z+=2; continue;}
				}
			}
			if($s!='' && $t==1) $v[$i][$s]=$b;
			$this->Records=$v;
			$qc[$table][$qss]->Records=$v;*/

			//как не странно - нижний способ работает быстрее чем верхний, т.е. множественные explod-ы и под-explod-ы работают быстрее линейного перебора всех символов
			$fs=filesize(FTEMP.$table.'/'.$qss);
			if($fs>0){$f=fopen(FTEMP.$table.'/'.$qss,'r');$tmp=fread($f,$fs);fclose($f);} else $tmp='';			
			$tmp=explode(SEP1,$tmp);
			$i=0;
			$qc[$table][$qss]->Records=Array();
			if($fs!=0) foreach($tmp AS $tm){
				$qc[$table][$qss]->Records[$i]=Array();
				$tm=explode(SEP2,$tm);
				foreach($tm AS $t){
					$t=explode(SEP3,$t);
					//if(empty($t[1])) $t[1]='';
					$qc[$table][$qss]->Records[$i][$t[0]]=$t[1];
				}
				$i++;				
			}
			$this->Records=$qc[$table][$qss]->Records;
			
			
			if($GLOBALS["iqs"]) etf('query');
			return 1;
		}
		if($type==1 && isset($qc[$table][$qss])){
			$this->Records=$qc[$table][$qss]->Records;
			if($GLOBALS["iqs"]) etf('query');
			return 1;
		}
	}
	if ($qs == ""){
		if($GLOBALS["iqs"]) etf('query');
		return 0;
	}
	if (!$this->connect()) {
		if($GLOBALS["iqs"]) etf('query');
		return 0;
	};
	if ($this->Query_ID) {
		$this->free();
	}
	global $cq_cache;
	$do_sql=true;
	if($cache_sql2){
		if(empty($qss)) $qss=md5($qs);
		if(isset($cq_cache[$qss]) && $cache_sql2){
			$this->Query_ID=$cq_cache[$qss]->qid;
			$this->Errno=$cq_cache[$qss]->errno;
			$this->Error=$cq_cache[$qss]->error;
			$go_debug=false;
			$do_sql=false;
		}
	}
	if($do_sql) {
		if($this->dtype=='mysql'){
			$this->Query_ID = mysql_query($qs,$this->Link_ID);
			$this->Errno = mysql_errno();
			$this->Error = mysql_error();
		}
		if($this->dtype=='mysqli'){
			$this->Query_ID = $this->Link_ID->query($qs);
			$this->Errno = $this->Link_ID->errno;
			$this->Error = $this->Link_ID->error;
		}
		if($this->dtype=='postgre'){
			$this->Query_ID=pg_query($this->Link_ID,$qs);
			$this->Errno  = pg_result_status($this->Query_ID);
			$this->Error  = pg_last_error($this->Link_ID);
		}
		if($cache_sql2){
			if(empty($qss)) $qss=md5($qs);
			$cq_cache[$qss]->qid=$this->Query_ID;
			$cq_cache[$qss]->errno=$this->Errno;
			$cq_cache[$qss]->error=$this->Error;
		}
		$go_debug=true;
	}
	if (!$this->Query_ID) {
		$this->halt("Invalid SQL: ".$qs);
	} else {
		if($debug && $go_debug){
			if($this->dtype=='mysql' && is_resource($this->Query_ID)) $qn=mysql_num_rows($this->Query_ID);
			else if($this->dtype=='mysqli' && is_object($this->Query_ID)) $qn=$this->Query_ID->num_rows;
			else if($this->dtype=='postgre' && is_resource($this->Query_ID)) $qn=pg_num_rows($this->Query_ID);
			else $qn=1;
			if(empty($tqrow)) $tqrow=1;
			else $tqrow=$tqrow+$qn;
		}
	}
	if($type==1 && !file_exists(FTEMP.$table.'/'.$qss)){
		if(!file_exists(FTEMP)){
			mkdir(FTEMP);
			chmod(FTEMP,DEF_DRMOD);
		}
		if(!file_exists(FTEMP.$table)){
			mkdir(FTEMP.$table);
			chmod(FTEMP.$table,DEF_DRMOD);
		}
		if(isset($this->Records)) unset($this->Records);
		$stat=true;$str='';$i=0; $this->Records=Array();
		while($stat){
			if($this->dtype=='mysql') $tr = mysql_fetch_array($this->Query_ID,MYSQL_ASSOC);
			if($this->dtype=='mysqli') $tr = $this->Query_ID->fetch_array(MYSQLI_ASSOC);
			if($this->dtype=='postgre') $tr=pg_fetch_array($this->Query_ID, Null, PGSQL_ASSOC);
			$stat = is_array($tr);
			if($stat){
				if(!empty($str)) $str.=SEP1;
				$this->Records[$i]=Array();
				$first=true;
				foreach($tr AS $var=>$value){
					$this->Records[$i][$var]=$value;
					if(!$first) $str.=SEP2;
					$str.=$var.SEP3.$value;
					$first=false;
				}
				$i++;
			}
		}
		$f=fopen(FTEMP.$table.'/'.$qss,'w');
		fwrite($f,$str);
		fclose($f);
		chmod(FTEMP.$table.'/'.$qss,DEF_CHMOD);
		$qc[$table][$qss]->Records=$this->Records;
		$this->free();
	}
	if($go_debug) if(empty($tquery)) $tquery=1; else $tquery++;
	if($debug && $go_debug){
		$mtime = microtime();$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];
		if(empty($aquery)) $aquery=Array();
		$i=count($aquery);
		$aquery[$i]=new stdClass();
		$aquery[$i]->s=$qs;
		$aquery[$i]->y=$otype;
		$aquery[$i]->n=$qn;
		$aquery[$i]->t=substr(($mtime-$mtime2),0,5);
		$aquery[$i]->z=$table;
		global $bquery,$cquery,$dquery,$curf,$equery;
		global $cur_part,$pdebug;
		$pdebug[$cur_part]->aqs[]=$aquery[$i];
		if(!isset($pdebug[$cur_part]->time)) $pdebug[$cur_part]->time=0;
		if(!isset($pdebug[$cur_part]->num)) $pdebug[$cur_part]->num=0;
		$pdebug[$cur_part]->time+=$aquery[$i]->t;
		$pdebug[$cur_part]->num+=$aquery[$i]->n;
		$equery[$curf][$qs]=new stdClass();
		$tmp=&$equery[$curf][$qs];
		if(empty($tmp) || !isset($tmp->count)){
			$tmp->count=1;
			$tmp->num=$qn;
			$tmp->time=$aquery[$i]->t;
		} else {
			$tmp->count++;
			$tmp->num+=$qn;
			$tmp->time+=$aquery[$i]->t;
		}
		if($file==''){
			$trace=debug_backtrace();
			//for($i2=count($trace)-1;$i2>0;$i2--){
			for($i2=0;$i2<count($trace)-1;$i2++){
				if(!strstr($trace[$i2]["file"],'db.inc')) break;
			}
			$file=$trace[$i2]["file"];
			$line=$trace[$i2]["line"];
			//echo $trace[count($trace)-2]["file"].' : '.$trace[count($trace)-2]["line"].'<br>';
		}
		$aquery[$i]->l=$line;
		$aquery[$i]->f=$file;
		if(!isset($bquery[(string)$file.' => '.$line])) $bquery[(string)$file.' => '.$line]=(int)(($mtime-$mtime2)*100000); else {
			$bquery[(string)$file.' => '.$line]+=(int)(($mtime-$mtime2)*100000);
		}
		if(!isset($cquery[(string)$file.' => '.$line])) $cquery[(string)$file.' => '.$line]=1; else $cquery[(string)$file.' => '.$line]++;
		if(!isset($dquery[(string)$file.' => '.$line])) $dquery[(string)$file.' => '.$line]=$qn; else $dquery[(string)$file.' => '.$line]+=$qn;
	}
	if($GLOBALS["iqs"]) etf('query');
	return $this->Query_ID;
}

}

//$max_connect=2;
$max_connect=1;
for($i=1;$i<=$max_connect;$i++){
	if($i==1) $id=''; else $id=$i;
	$GLOBALS["db".$id]=new DB_Sql;
	if(empty($database_type)){
		$database_type='mysql';
		if(function_exists('mysqli_connect')) $database_type='mysqli';
	}
	$GLOBALS["db".$id]->dtype=$database_type;
	$GLOBALS["db".$id]->Host=$server;
	$GLOBALS["db".$id]->Database=$database;
	$GLOBALS["db".$id]->User=$username;
	$GLOBALS["db".$id]->Password=$password;
	$GLOBALS["db".$id]->Halt_On_Error='yes';
	if(isset($port)) $GLOBALS["db".$id]->port=$port;
	if($database_type=='mysql' || $database_type=='mysqli'){
		//if(empty($_POST["db_server"])){
			if(isset($correct_utf) && $correct_utf && $i==1){
				$GLOBALS["db".$id]->query("SET NAMES `utf8` COLLATE `utf8_general_ci`");
			}
		//}
	}
	//$GLOBALS["db".$id]->query("SET NAMES `utf8` COLLATE `utf8_unicode_ci`");
	/*$GLOBALS["db".$id]->query("SET NAMES `utf8` COLLATE `utf8_general_ci`");
	$GLOBALS["db".$id]->query("SET collation_connection = 'utf8_general_ci'");
	$GLOBALS["db".$id]->query("SET collation_database = 'utf8_general_ci'");	
	$GLOBALS["db".$id]->query("SET collation_server = 'utf8_general_ci'");		
	$GLOBALS["db".$id]->query("SET character_set_client = 'utf8'");
	$GLOBALS["db".$id]->query("SET character_set_connection  = 'utf8'");
	$GLOBALS["db".$id]->query("SET character_set_database = 'utf8'");
	$GLOBALS["db".$id]->query("SET character_set_results = 'utf8'");*/
}

unset($username);
unset($password);

?>