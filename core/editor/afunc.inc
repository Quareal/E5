<?php

function add_ex($module,$ex_name,$ex_sname,$ex_major,$new_zone,$prefix=''){
	global $db,$user;
	$id=$module;
	$module_id=$module;
	if (/*!check_mod($id,'edit') && */!check_ex(0,'add',$id)) return false;
	if(empty($_POST[$prefix.'ex_public'])) $ex_public=0; else $ex_public=1;
	$ex_uin=uuid();
	$db->query("INSERT INTO ex_module (ex_name, ex_sname, ex_module, ex_major, ex_public, ex_uin)
			VALUES ('$ex_name', '$ex_sname', $id, $ex_major, $ex_public, '$ex_uin')",3,'ex_module');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	$GLOBALS["lex_ex"]=$sid;
	$GLOBALS["cancel"]=false;
	if(!empty($GLOBALS["cex".$id])) $old_cex=$GLOBALS["cex".$id];
	$cex=$sid;	SetCookie('cex'.$id,$sid, time()+60*60*24*30,'/');
	
	//добавляем экземпляры таблиц
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id",1,'main_table');
	$tables_to_del=Array(); $tables_to_del2=Array();
	if(!empty($tbl)) foreach($tbl AS $tb){
		if(empty($_POST[$prefix."rb".$tb["table_id"]])  || $_POST[$prefix."rb".$tb["table_id"]]==0){
			if(empty($_POST[$prefix."nex".$tb["table_id"]])) $_POST[$prefix."nex".$tb["table_id"]]=$tb["table_name"].' 1';
			$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
			VALUES (".$tb["table_id"].", $id, '".$_POST[$prefix."nex".$tb["table_id"]]."')",3,'ex_table');
			getrow($db,"SELECT LAST_INSERT_ID() as sid2");
			$sid2=$db->Record["sid2"];
			$tables_to_del[$sid2]=$sid2;
			$GLOBALS["lex_table"][$tb["table_id"]]=$sid2;
		} else {
			$sid2=$_POST[$prefix."oex".$tb["table_id"]];
			$GLOBALS["lex_table"][$tb["table_id"]]=$sid2;
		}
		$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
		VALUES ($id, ".$tb["table_id"].",$sid2,$sid)",3,'ex_group');
		getrow($db,"SELECT LAST_INSERT_ID() as sid3");
		$sid3=$db->Record["sid3"];
		$tables_to_del2[$sid3]=$sid3;
	}		
	echo '<br><br>';
	
	echo insert_values(0,$module_id,$sid,0,$prefix);
	
	$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$id");
	$tid=''; if(!empty($tbls)) foreach($tbls AS $tbl){if(!empty($tid)) $tid.=','; $tid.=$tbl["table_id"];}
	if(!empty($tid)) $tbls2=getall($db,"SELECT * FROM table_sub WHERE sub_table1 IN ($tid)");
	$tid=''; if(!empty($tbls2)) foreach($tbls2 AS $tbl2) {if(!empty($tid)) $tid.=','; $tid.=$tbl2["sub_table2"];}
	if(!empty($tid)) $tbls3=getall($db,"SELECT * FROM main_table WHERE table_module!=$id AND table_id IN ($tid)");
	$subtable=Array();if(!empty($tbls3)) foreach($tbls3 AS $tbl3) $subtable[$tbl3["table_module"]]=$tbl3["table_name"];	
	
	$all_subtables=get_crossmodule_subtable($module,1);
	if(!empty($all_subtables)) foreach($all_subtables AS $var=>$value){
		echo insert_values(0,$var,$sid,0,$prefix);
	}
	if($new_zone!=0){
		$db->query("INSERT INTO ex_zone (ex_zone,ex_module,ex_module2)
		VALUES($new_zone,$cex,$id)",3,"ex_zone");
	}
	
	if($GLOBALS["cancel"]){
		$db->query("DELETE FROM ex_module WHERE ex_id=$sid",3,"ex_module");
		if(!empty($tables_to_del)) foreach($tables_to_del AS $ttid) $db->query("DELETE FROM ex_table WHERE ex_id=$ttid");
		if(!empty($tables_to_del2)) foreach($tables_to_del2 AS $ttid) $db->query("DELETE FROM ex_group WHERE ex_id=$ttid");
		if(!empty($old_cex)) {$cex=$old_cex; SetCookie('cex'.$id,$old_cex, time()+60*60*24*30,'/');} else {$cex=0; SetCookie('cex'.$id,0, time()+60*60*24*30,'/');}
		del_row($sid);
		echo '<div align="center"><h2 style="color:#FF0000;">'.lng('Adding failed because').': '.$GLOBALS["cancel"].'</h2></div>';
		$cex=0;
	} else {
		//добавления доступа, в случае, если экземпляр создал не SuperUser
		global $user;
		if(!$user->super){
			$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_edit,perm_del)
								VALUES	(1,2,$user->id,$sid,2,2,2)",3,"auth_perm");
		}
	}	
	return $cex;
}

function add_ex_form($module,$select_zone=0,$prefix=''){
	global $db, $user, $zone;
	getrow($db,"SELECT * FROM main_module WHERE module_id=$module",1,"main_module");
	$module_name=$db->Record["module_name"];
	$module_sname=$db->Record["module_sname"];
	getrow($db,"SELECT count(ex_id) AS count FROM ex_module WHERE ex_module=$module",1,"main_ex");
	if(empty($db->Record["count"])) $module_count=1; else $module_count=$db->Record["count"]+1;
	getrow($db,"SELECT * FROM main_table WHERE table_module=$module AND table_bold=1",1,"main_table");
	if(!empty($db->Record["table_id"])) $module_major=$db->Record["table_id"]; else $module_major=0;
	$id=$module;
	if(!check_mod($id,'edit') && !check_ex(0,'add',$id)) return false;
	
	$vars['only_body']=1;
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'ex_name','value'=>$module_name.' '.$module_count,'title'=>lng('Name'),'addon'=>' OnBlur="translate2(this,'.$prefix.'ex_sname);"');
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'ex_sname','value'=>latinica($module_name).$module_count,'title'=>lng('Unique special name in English'));
	$vars['section']['main']['fields'][]=Array('type'=>'checkbox','name'=>$prefix.'ex_public','value'=>0,'caption'=>lng('Allow open access to data (for updates child systems)'));
	$vars['section']['main']['fields'][]=Array('type'=>'static','content'=>input_form(0,$id,0,0,0,0,0,0,Array(),$prefix));
	
	//echo '<p>'.lng('Name').'<br><input name="'.$prefix.'ex_name" type="text" value="'.$module_name.' '.$module_count.'" OnBlur="translate2(this,'.$prefix.'ex_sname);"></p>';
	//echo '<p>'.lng('Unique special name in English').'<br><input name="'.$prefix.'ex_sname" type="text" value="'.latinica($module_name).$module_count.'"></p>';
	
	//echo input_form(0,$id,0,0,0,0,0,0,Array(),$prefix);
	
	if($select_zone){
		//echo '<p>'.lng('Connection with site').':<br><select name="'.$prefix.'new_zone"><option value="0" selected>'.lng('No').'</option>';
		//foreach($zone AS $z) if(/*empty($tzo[$z["zone_id"]]) && */empty($z["zone_redirect"]) && check_zone($z["zone_id"],'view') && $z["zone_module"]!=-1 && $z["zone_module"]!=-2) echo '<option value="'.$z["zone_id"].'">'.$z["zone_name"].'</option>';
		//echo '</select></p>';
		$items=Array(
			Array('id'=>'0','value'=>lng('No'))
		);
		foreach($zone AS $z) if(empty($z["zone_redirect"]) && check_zone($z["zone_id"],'view') && $z["zone_module"]!=-1 && $z["zone_module"]!=-2){
			$items[]=Array('id'=>$z['zone_id'],'value'=>$z['zone_name']);
		}
		$vars['section']['main']['fields'][]=Array('type'=>'select','name'=>$prefix.'new_zone','value'=>0,'items'=>$items,'title'=>lng('Connection with site'));
	}
	
	$tid=0;
	
	$all_subtables=get_crossmodule_subtable($module,1);
	
	if(!empty($all_subtables)) foreach($all_subtables AS $var=>$value)if($var!=$id){
		$z=input_form(0,$var,0,0,0,0,0,1,Array(),$prefix);
		if($z!=''){
			$tid++;
			$vars['section']['subtable'.$tid]['hidden']=1;
			$vars['section']['subtable'.$tid]['icon']='back_config';
			$vars['section']['subtable'.$tid]['title']=lng('Subtable parameters').' «'.$value.'»';
			$vars['section']['subtable'.$tid]['fields'][]=Array('type'=>'static','content'=>$z);
			//echo '<h2 OnClick="showhide(\'additional_table'.$tid.'\')" style="cursor: pointer;">'.lng('Subtable parameters').' «'.$value.'»</h2>';
			//echo '<div id="additional_table'.$tid.'" style="display: none;">';
			//echo $z;
			//echo '</div>';
		}
	}
	
	getrow($db,"SELECT * FROM main_table WHERE table_module=$id  AND table_extype=0",1,'main_table');
	//echo '<h2 OnClick="showhide(\'additional_param\')" style="cursor: pointer;">'.si('back_table').lng('Tables settings').'</h2>';
	$vars['section']['table_settings']['title']=lng('Tables settings');
	$vars['section']['table_settings']['icon']='back_table';
	if(empty($db->Record)) $vars['section']['table_settings']['hidden']=1;
	
	//if(!empty($db->Record)) echo '<div id="additional_param">';	
	//else echo '<div id="additional_param" style="display: none;">';	
	$he=true;
	
	/*echo '<p>'.lng('Major table for this section').':<br>';
	echo '<select name="'.$prefix.'ex_major"><option value="0">'.lng('Coincides with the main table of the module').'</option>';	
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id ORDER BY table_name",1,'main_table');
	if(!empty($tbl)) foreach($tbl AS $tb){
		$add='';
		if($tb["table_id"]!=$module_major) echo '<option value="'.$tb["table_id"].'"'.$add.'>'.$tb["table_name"].'</option>';
	}
	echo '</select></p>';*/
	$items=Array(
			Array('id'=>0,'value'=>lng('Coincides with the main table of the module'))
	);
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id ORDER BY table_name",1,'main_table');
	if(!empty($tbl)) foreach($tbl AS $tb){
		if($tb["table_id"]!=$module_major) $items[]=Array('id'=>$tb['table_id'],'value'=>$tb['table_name']);
	}
	$vars['section']['table_settings']['fields'][]=Array('type'=>'select','name'=>$prefix.'ex_major','value'=>$module_major,'items'=>$items,'title'=>lng('Major table for this section'));
	
	$he=false;
	$additional='';
	$texs_all=getall5($db,"SELECT * FROM ex_group WHERE ex_module=$id","ex_ex1");
	$ext_all=getall5($db,"SELECT * FROM ex_table WHERE ex_module=$id","ex_table");
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id ORDER BY table_extype, table_name",1,'main_table');
	if(!empty($tbl)) foreach($tbl AS $tb){
		if(!empty($ext_all[$tb["table_id"]])) $ext=$ext_all[$tb["table_id"]]; else $ext=Array();
		$sid=count($ext);
		if(check_tbl($tb["table_id"],'view')){
			$add1=' checked';$add2='';
			if($tb["table_extype"]==2){$add2=' checked'; $add1='';}
			if($tb["table_extype"]!=0 && !$he){
				$he=true;
				$additional.='<h2 OnClick="showhide(\'additional_param2\')" style="cursor: pointer;">'.si('point').lng('Redefinition of tables').'</h2>';
				$additional.='<div id="additional_param2" style="display: none;">';
			}
			if(empty($ext)) $add1=' checked';
			$additional.='<p><b>'.lng('Table').' «'.$tb["table_name"].'» '.lng('participates as').'</b><br>
				<input type="radio" class="checkbox" id="'.$prefix.'rb'.$tb["table_id"].'" name="'.$prefix.'rb'.$tb["table_id"].'" value="0"'.$add1.'> '.lng('new instance').': <input class="button" type="text" name="'.$prefix.'nex'.$tb["table_id"].'" value="'.$tb["table_name"].' '.($sid+1).'" OnClick="document.exadd.'.$prefix.'rb'.$tb["table_id"].'[0].checked=true;">';
			$close_select=false;
			if(!empty($ext)){
				$additional.=' '.lng('or existing').': <select name="'.$prefix.'oex'.$tb["table_id"].'" class="button" OnClick="document.exadd.'.$prefix.'rb'.$tb["table_id"].'[1].checked=true;">';
				$close_select=true;
			}
			$hext=false;
			foreach($ext AS $ex){
				if(!empty($texs_all[$ex["ex_id"]])) $texs=$texs_all[$ex["ex_id"]]; else $texs=Array();
				$allow=false;
				if(!empty($texs)){
					foreach($texs AS $tex) if(check_ex($tex["ex_ex2"],'view')){$allow=true; break;}
				} else if($user->super) $allow=true; //даёт доступ суперпользователю к просмотру экземпляров таблиц, не закреплённых для конкретного экземпляра модуля
				if($allow){
					$hext=true;
					$additional.='<option value="'.$ex["ex_id"].'">'.$ex["ex_name"].'</option>';
				}
			}
			if($close_select) $additional.='</select>';
			if($hext) $additional.='<input type="radio" class="checkbox" id="'.$prefix.'rb'.$tb["table_id"].'" name="'.$prefix.'rb'.$tb["table_id"].'" value="1"'.$add2.'>';
			$additional.='</p>';
		} else {
			if($tb["table_extype"]!=2 || empty($ext)){
				$additional.='<input type="hidden" name="'.$prefix.'rb'.$tb["table_id"].'" value="0"><input type="hidden" name="'.$prefix.'nex'.$tb["table_id"].'" value="'.$tb["table_name"].' '.($sid+1).'">';
			} else {
				foreach($ext AS $first) break;
				$additional.='<input type="hidden" name="'.$prefix.'rb'.$tb["table_id"].'" value="1"><input type="hidden" name="'.$prefix.'oex'.$tb["table_id"].'" value="'.$first["ex_id"].'">';
			}
		}
	}
	if($he) $additional.='</div>';
	$vars['section']['table_settings']['fields'][]=Array('type'=>'static','content'=>$additional);
	echo shell_tpl_admin('block/form', $vars);
	//echo '</div>';
}

function add_user_form($help='',$prefix='',$prevars=Array(),$vars=Array(),$return_vars=0){
	global $user;
	if(!empty($prevars)) foreach($prevars AS $var=>$value){
		if($var=='session_lifetime') $var='user_session_lifetime';
		if($var=='session_multy') $var='user_session_multy';
		$$var=$value;
	} else {
		$user_login='';
		$user_pwl='';
		$user_pwlcode=0;
		$user_email='';
		$user_fixedip='';
		$user_name='';
		$user_session_lifetime=-1;
		$user_session_multy=-1;
	}
	if(!$return_vars) $vars['only_body']=1;
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'user_login','value'=>$user_login,'title'=>lng('Login'));
	if(empty($user_pwl)){
		$vars['section']['main']['fields'][]=Array('type'=>'password','name'=>$prefix.'user_pwl','value'=>'','title'=>lng('Password'));
		$vars['section']['main']['fields'][]=Array('type'=>'password','name'=>$prefix.'user_pwl2','value'=>'','title'=>lng('Password (confirm)'));
		$vars['section']['main']['fields'][]=Array('type'=>'select','name'=>$prefix.'user_pwlcode','title'=>lng('Method of storing password'),'items'=>Array(
			Array('id'=>0,'value'=>lng('In its original form')),
			Array('id'=>1,'value'=>lng('Encrypted'))
		),'value'=>$user_pwlcode);
	} else {
		if($user->super==1 && $user_pwlcode==0) $vars['section']['main']['fields'][]=Array('type'=>'static','content'=>'<div>'.lng('Current password').': '.$user_pwl.'</div>');
		$vars['section']['main']['fields'][]=Array('type'=>'checkbox_folder','name'=>$prefix.'change_password','caption'=>lng('Change password?'),'sub'=>Array(
			Array('type'=>'password','name'=>$prefix.'user_pwl','value'=>'','title'=>lng('Password')),
			Array('type'=>'password','name'=>$prefix.'user_pwl2','value'=>'','title'=>lng('Password (confirm)')),
			Array('type'=>'select','name'=>$prefix.'user_pwlcode','title'=>lng('Method of storing password'),'items'=>Array(
				Array('id'=>0,'value'=>lng('In its original form')),
				Array('id'=>1,'value'=>lng('Encrypted'))
			))
		));
	}
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'user_email','value'=>$user_email,'title'=>lng('Email password recovery'));		
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'user_name','value'=>$user_name,'title'=>lng('Username'));
	$vars['section']['main']['fields'][]=Array('type'=>'textarea','name'=>$prefix.'user_fixedip','value'=>$user_fixedip,'title'=>lng('IP-configuration'));
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'user_session_lifetime','value'=>$user_session_lifetime,'title'=>lng('Maximum session idle time (in minutes, 0 - infinity, -1 - use system defaults)'));
	$vars['section']['main']['fields'][]=Array('type'=>'select','name'=>$prefix.'user_session_multy','value'=>$user_session_multy,'title'=>lng('Configure using multiple sessions (override system settings)'),'items'=>Array(
								Array('id'=>-1,'value'=>lng('use system settings')),
								Array('id'=>0,'value'=>lng('deny')),
								Array('id'=>1,'value'=>lng('allow'))
							));
	if($return_vars) return $vars;
	else return shell_tpl_admin('block/form', $vars);
}

function show_ace_editor_simple($lang,$name,$value,$width='95%',$height='600px',$style='',$theme='crimson_editor'){
	global $use_ace;
	if($use_ace || 1==1){
	echo '
	<style>.ace-'.$name.'{ border: 1px solid #DDDDDD; display: block; margin: 0px; overflow: hidden; position: relative; width: '.$width.'; height: '.$height.';'.$style.'}</style>
	<input type="hidden" value="" name="'.$name.'" id="'.$name.'">
	<pre id="editor_'.$name.'" class="ace-'.$name.'">'.htmlspecialchars($value).'</pre>';
	if(empty($GLOBALS["include_ace"])) echo '<script src="'.$GLOBALS["base_root"].'/files/editor/ace/ace.js" type="text/javascript" charset="utf-8"></script>';
	$GLOBALS["include_ace"]=1;
	echo '
	<script>
	    var editor_'.$name.' = ace.edit("editor_'.$name.'");
	    editor_'.$name.'.setTheme("ace/theme/'.$theme.'");
	    editor_'.$name.'.getSession().setMode("ace/mode/'.$lang.'");
	    editor_'.$name.'.setShowPrintMargin(false);
	    editor_'.$name.'.setHighlightActiveLine(false);
	    editor_'.$name.'.setDisplayIndentGuides(false);
	    editor_'.$name.'.getSession().setUseWrapMode(true);
	    if(editors==undefined){ var editors=Array(); }
	    var en="'.$name.'";
	    editors[en]=editor_'.$name.';
	    $(document).ready(function(){
		    $("#'.$name.'").parents("form").get(0).onsubmit=function(e){
		    	    for (var key in editors){
			    	var x=editors[key].getValue();
			    	$("#"+key).val(x);
		    	    }
		    };
	     });

	</script>
	';	
	} else {
		$data=str_replace('</textarea>','{-{-{/textarea>',$value);
		$data=htmlspecialchars($data,ENT_QUOTES);
		echo '<textarea name="'.$name.'" id="'.$name.'" style="width: '.$width.'; height: '.$height.';">'.$data.'</textarea>';
	}
}

function ide($data,$t,$id,$id2,$part_type,$cname='part_body',$type='part',$use_ace=1,$can_save=false,$cur_table=0,$cur_part=0,$vars=Array(),$edit_row=0,$edit_col=0){
	global $db;
	if(empty($GLOBALS["include_ace"])) echo '<script src="'.$GLOBALS["base_root"].'/files/editor/ace/ace.js" type="text/javascript" charset="utf-8"></script>';
	if(is_object($cur_table)) $cur_table=$cur_table->id;
	if(is_object($cur_part)) $cur_part=$cur_part->id;
	if(is_object($id2)) $id2=$id2->id;
	$GLOBALS["include_ace"]=1;
	$width='95%';
	$height='600';
	if($type=='table_part'){
		$width='100%';
		$height='300';
	}
	if($type=='table_col'){
		$width='95%';
		$height='300';
	}
	if($type=='terminal'){
		$width='99%';
		$height='400';
	}

	$ct=0;
	$cm=0;
	$first=false;
	echo '<script>';
	if($type=='part'){
		if($t==-1){
			if($part_type==4) echo 'isImport=1;';
			$tmp=getrow($db,"SELECT part_table, timer_type FROM main_part WHERE part_id=".$id2);
			$ct=$tmp['part_table'];
			if($tmp['timer_type']!=0) echo 'isCron=1;';
			$cm=$id;
		} else {
			echo 'selectComponentType='.$t.';';
			echo 'selectComponent='.$id2.';';
		}
		if(empty($cur_part)) echo 'selectPart='.$id2.';';
	}
	if($cur_part!=0) echo 'selectPart='.$cur_part.';';
	if($cur_table!=0){
		echo 'currentTable='.$cur_table.';';
	}
	if($type=='form'){
		// для использования в качестве компонента формы (form.module_editor)
		$ct=$id2;
		$cm=$id;
	}
	if($type=='terminal'){
		
	}
	if($type=='table_part'){
		$ct=$id2;
		$cm=$id;
		if($cname=='table_top') echo 'isTableTop=1;';
		if($cname=='table_bottom') echo 'isTableBottom=1;';
		if($cname=='table_onedit') echo 'isTableEdit=1;';
	}
	if($type=='table_col'){
		$ct=$id2;
		$cm=$id;
		if(!empty($part_type)) echo 'selectCol='.$part_type.';';
		if($cname=='col_oninsert') echo 'isColEdit=1;';
		if($cname=='col_onform') echo 'isColForm=1;';
		if($cname=='col_onshow') echo 'isColShow=1;';
	}
	if($cm){
		echo 'selectModule='.$cm.';';
		echo 'selectModuleSname=\''.getrowval("SELECT module_sname FROM main_module WHERE module_id=$cm","module_sname").'\';';
	}
	if($ct) echo 'selectTable='.$ct.';';
	echo '</script>';
	
	show_ace_editor('html',$cname,$data,$width,$height.'px','',$can_save,$cur_part,$vars,$edit_row,$edit_col);	
}

function show_ace_editor($lang,$name,$value,$width='95%',$height='600px',$style='',$can_save=false,$replace_part=0,$vars=Array(),$edit_row=0,$edit_col=0){
	global $use_ace,$ace_init;
	if(empty($ace_init)){
		$ace_init=1;
		//echo '<link rel="stylesheet" type="text/css" href="'.$GLOBALS['base_root'].'/files/editor/meditor/styles.css" title="default" />';
		//echo '<script type="text/javascript" src="'.$GLOBALS['base_root'].'/files/editor/meditor/script.js"></script>';
		//echo '<script type="text/javascript" src="'.$GLOBALS['base_root'].'/files/editor/meditor/insert-manager.js"></script>';
	}
	$is_admin=($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1);
	if(is_object($edit_col)) $edit_col=$edit_col->id;
	if(is_object($edit_row)) $edit_row=$edit_row->id;
	echo '
	<style>
		.ace-'.$name.'{
			border: 1px solid #DDDDDD;
			display: block;
			margin: 0px;
			overflow: hidden;
			border-top: 0px;
			position: relative;
			width: '.$width.';
			height: '.$height.';
			'.$style.'
		}
	</style>	
	<input type="hidden" value="" name="'.$name.'" id="'.$name.'">
	<div id="'.$name.'_editor_shadow" class="shadowbox"></div>
	<div id="'.$name.'_scroll_box" class="fakeScroll" OnScroll="fakeScroll(this,\''.$name.'\')" OnMouseUp="fakeScroll(this,\''.$name.'\')" style="width: '.$width.'"><div id="'.$name.'_scroll_content" style="width: 3000px;">&nbsp;</div></div>
	<div id="'.$name.'_toolbar" class="toolbar" style="width: '.$width.'">
		<div class="container">
			<div style="padding-left: 6px; padding-top: 4px;">
					'.($can_save?'<img src="'.$GLOBALS["base_root"].'/files/editor/classic/save.png" class="btn" OnClick="form_submit(\''.$name.'\', '.$edit_row.', '.$edit_col.');">':'').'
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/value.png" class="btn" OnClick="show_op_editor(\''.$name.'\',0);">
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/brackets.png" class="btn" OnClick="paste_editor(editor_'.$name.',\'[]\',1,1);">
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/if.png" class="btn" OnClick="paste_editor(editor_'.$name.',\'[if ][/if]\',4,5);">
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/tree.png" class="btn" OnClick="paste_editor(editor_'.$name.',\'[tree ][/tree]\',6,7);">
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/noscript.png" class="btn" OnClick="paste_editor(editor_'.$name.',\'[~  ~]\',3,3);">
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/comment.png" class="btn" OnClick="paste_editor(editor_'.$name.',\'[*  *]\',3,3);">
					'.($is_admin?'<img src="'.$GLOBALS["base_root"].'/files/editor/classic/file2.png" class="btn" OnClick="show_elfinder(function(url) {paste_editor(editor_'.$name.',url,url.length,0);} );">':'').'
					<img src="'.$GLOBALS["base_root"].'/files/editor/classic/update.png" class="btn" OnClick="refresh_data();">
			</div>
		</div>
	</div>
	<pre id="editor_'.$name.'" class="ace-'.$name.'">'.htmlspecialchars($value).'</pre>';

	if(empty($GLOBALS["include_ace"])) echo '<script src="'.$GLOBALS["base_root"].'/files/editor/ace/ace.js" type="text/javascript" charset="utf-8"></script>';
	$GLOBALS["include_ace"]=1;
	if(!empty($vars)) $v='additional_vars=Array(\''.implode("','",$vars).'\');';
	else $v='';
	if(empty($edit_col)) $edit_col='0';

	echo '
	<script>
	    var noScroll=false;
	    var noScroll2=false;
	    '.$v.'
	    var part_id='.($replace_part!=0?$replace_part:(!empty($_POST["id2"])?$_POST["id2"]:(!empty($_GET["id2"])?$_GET["id2"]:0))).';
	    var save_part='.(!empty($_POST["id2"])?$_POST["id2"]:(!empty($_GET["id2"])?$_GET["id2"]:0)).';
	    var editor_'.$name.' = ace.edit("editor_'.$name.'");
    	    var en="'.$name.'";
	    if(x_cmd==undefined) var x_cmd=load_json(jsq2+"/ajax?action=get_im_json&name=cmd");
	    if(x_component==undefined) var x_component=load_json(jsq2+"/ajax?action=get_im_json&name=component");
	    if(x_widget==undefined) var x_widget=load_json(jsq2+"/ajax?action=get_im_json&name=widget");
	    if(selectModuleSname){
	    	if(x_module==undefined) var x_module=load_json(jsq2+"/ajax?action=get_im_json&name="+selectModuleSname);
	    } else var x_module=0;
	    editor_'.$name.'.setTheme("ace/theme/crimson_editor");//adaptive
	    //editor_'.$name.'.setTheme("ace/theme/chrome");
	    //editor_'.$name.'.setTheme("ace/theme/dawn");
	    editor_'.$name.'.getSession().setMode("ace/mode/'.$lang.'");
	    
	    editor_'.$name.'.setShowPrintMargin(false);
	    /*editor_'.$name.'.setOptions({
			enableBasicAutocompletion: false,
		        enableSnippets: false,
		        enableLiveAutocompletion: false
	    });*/
	    editor_'.$name.'.setHighlightActiveLine(false);
	    editor_'.$name.'.setDisplayIndentGuides(false);
	    editor_'.$name.'.setShowFoldWidgets(false);
	    editor_'.$name.'.renderer.setShowGutter(false);
	    editor_'.$name.'.renderer.onResize(true);
	    editor_'.$name.'.can_save='.($can_save?'1':'0').';
	    editor_'.$name.'.edit_row='.$edit_row.';
	    editor_'.$name.'.edit_col='.$edit_col.';
	    editor_'.$name.'.editor_name=\''.$name.'\';
	    //editor_'.$name.'.setOption("enableBasicAutocompletion", true);
	    //editor_'.$name.'.autoComplete=false;
	    //editor_'.$name.'.autoIdent=false;
	    editor_'.$name.'.$blockScrolling=Infinity;
	    if(editors==undefined){ var editors=Array(); }
	    editors[en]=editor_'.$name.';	    
    	    editor_'.$name.'.getSession().on("change", function(){
			heightUpdateFunction("'.$name.'");
    	    });
    	    $(document).bind("scroll", function(){
			for (var key in editors) onScrollUpdate(key);
	    });	    
	    
	    /*var langTools = ace.require("ace/lib/lang");
	    langTools.completers=[];*/

	   /*var block_scroll_handle=false;
	   function fix_search_problem(editor_name){
    	    		if(block_scroll_handle){
	    	    		block_scroll_handle=false;
    	    			return false;
    	    		} else {
	    	    		block_scroll_handle=true;
    	    		}
    	    		var tns=noScroll;
    	    		var tns2=noScroll2;
    	    		noScroll=true;
    	    		noScroll2=true;
			var left=$("#editor_"+editor_name+" .ace_scroller").scrollLeft()-1;
			$("#editor_"+editor_name+" .ace_content").css("marginLeft","-"+left+"px");
			$("#editor_"+editor_name+" .ace_scrollbar-h").get(0).scrollLeft=left;
			if(left>0) $("#editor_"+editor_name+" .ace_scroller").addClass("ace_scroll-left");
			else $("#editor_"+editor_name+" .ace_scroller").removeClass("ace_scroll-left");
    	    		noScroll=tns;
    	    		noScroll2=tns2;
			$("#editor_"+editor_name+" .ace_scroller").scrollLeft(0);
	    }
    	    $("#editor_'.$name.' .ace_scroller").bind("scroll", function(){
			fix_search_problem("'.$name.'");
	    });*/
	      
       	    $("#editor_'.$name.' .ace_scrollbar-h").bind( "scroll", function(){
			onScrollReverseUpdate("'.$name.'");
	    });
	    
      	    $(window).bind( "resize", function(){
			onScrollUpdate("'.$name.'");
	    });
	    
      	    $(window).bind( "keydown", function(e){
			if(!e) e=window.event;
			if(e.keyCode==114){
				editor_'.$name.'.findNext();
				return false;
			}
	    });	   
	    
	    /*var allCommands = editor_'.$name.'.commands.byName; 
	    ace.commands.removeCommands(allCommands)
	    allCommands.findnext.bindKey = {win: "Ctrl-H", mac: "Cmd-H"}
	    editor_'.$name.'.commands.addCommand(allCommands.findnext)*/
	    
	    $(document).ready(function(){    
			for (var key in editors){
				var x=$("#"+key).val();
				//alert("#"+key);
				//alert(x);
				if(x && editors[key].getValue()!=x){
					editors[key].setValue(x);
					editors[key].clearSelection();
				}
			}
			heightUpdateFunction("'.$name.'");
			onScrollUpdate("'.$name.'");
			$("#'.$name.'").parents("*").onShow(function(){
				onScrollUpdate("'.$name.'");
			});
			var xsubmit=$("#'.$name.'").parents("form").get(0).onsubmit;
			$("#'.$name.'").parents("form").get(0).onsubmit=function(e){
				    if(xsubmit && xsubmit()==false) return false;
				    for (var key in editors){
				    	//document.getElementById(key).value=editors[key].getValue();
				    	var x=editors[key].getValue();
				    	$("#"+key).val(x);
				    }
			    '
			    /*.($can_save?(empty($edit_row)?'hotSave();':'hotSave_inline('.$edit_row.','.$edit_col.');'):'')*/
			    .'
			};
			manager_ready=true;
	   });	   
	   
	    //editor_'.$name.'.commands.removeCommand("find");
	    editor_'.$name.'.commands.removeCommand("scroll");
	    //editor_'.$name.'.setAutoScrollEditorIntoView(); 
    	    /*editor_'.$name.'.commands.addCommand({//disable default search dialog
		    name: "unfind",
		    bindKey: {
		        win: "Ctrl-f",
		        mac: "Command-f"
		    },
		    exec: function(editor, line) {
		    	
		        return false;
		    },
		    readOnly: true
		});*/
			
    	    editor_'.$name.'.commands.addCommand({
		    name: "check_move",
		    bindKey: "PageUp",
		    exec: function(editor, line) {
		    	var n=editor.selection.getCursor().row;
 			var x=parseInt($(window).height());
			var h=editor_'.$name.'.renderer.lineHeight;
		    	var z=Math.floor(x/h);
		        editor.gotoLine(n-z);
		        return true;
		    },
		    passEvent: false
		});
		
    	    editor_'.$name.'.commands.addCommand({
		    name: "check_moveS",
		    bindKey: "Shift-PageUp",
		    exec: function(editor, line) {
		    	var n=editor.selection.getCursor().row;
 			var x=parseInt($(window).height());
			var h=editor_'.$name.'.renderer.lineHeight;
		    	var z=Math.floor(x/h);
		        editor_'.$name.'.selection.selectTo(n-z,0);
		        return true;
		    },
		    passEvent: false
		});
		
    	    editor_'.$name.'.commands.addCommand({
		    name: "check_move2",
		    bindKey: "PageDown",
		    exec: function(editor, line) {
		    	var n=editor.selection.getCursor().row;
 			var x=parseInt($(window).height());
			var h=editor_'.$name.'.renderer.lineHeight;
		    	var z=Math.floor(x/h);
		        editor.gotoLine(n+z);
		        return true;
		    },
		    passEvent: false
		});
		
    	    editor_'.$name.'.commands.addCommand({
		    name: "check_move2S",
		    bindKey: "Shift-PageDown",
		    exec: function(editor, line) {
		    	var n=editor.selection.getCursor().row;
 			var x=parseInt($(window).height());
			var h=editor_'.$name.'.renderer.lineHeight;
		    	var z=Math.floor(x/h);
		        editor_'.$name.'.selection.selectTo(n+z,0);
		        return true;
		    },
		    passEvent: false
		});
		
    	    editor_'.$name.'.commands.addCommand({
		    name: "check_move3",
		    bindKey: "Home",
		    exec: function(editor, line) {
		    	var n=editor.selection.getCursor().row;
		        //editor.gotoLine(n+1);
			//editor.selection.moveCursorToPosition({column: 0, row: n});
			//editor.selection.moveTo({column: 0, row: n});
			//editor.focus();
			//editor.execCommand("gotolinestart");
			editor.gotoLine(n+1); 
			//editor.navigateLineStart(); // Navigate to end of line
			
			$("#editor_'.$name.' .ace_content").css("marginLeft","0px");
			$("#editor_'.$name.' .ace_scrollbar-h").get(0).scrollLeft=0;
			$("#editor_'.$name.' .ace_scroller").removeClass("ace_scroll-left");
			
		        return true;
		    },
		    passEvent: false
		});
		
		/*$(".ace_text-input").bind("select", function(e){
			alert(123);
		});*/
		
		editor_'.$name.'.getSession().selection.on("changeCursor", function(e){
			var line=editor_'.$name.'.selection.getCursor().row;
			var z=parseInt($("#editor_'.$name.'").position().top);
			var y=parseInt($(document).scrollTop());
			var x=parseInt($(window).height());
			var h=editor_'.$name.'.renderer.lineHeight;
			var toolbar_h=22;//40//35
			if(line*h-h<y-z+toolbar_h) $(document).scrollTop(line*h-h+z-toolbar_h);
			if(line*h+h*3>y-z+x) $(document).scrollTop(line*h+z-x+h*3);
			
			//overscroll on find next dialog
			/*var left=$("#editor_'.$name.' .ace_scroller").scrollLeft()-1;
			if(left>0){
				$("#editor_'.$name.' .ace_content").css("marginLeft","-"+left+"px");
				$("#editor_'.$name.' .ace_scrollbar-h").get(0).scrollLeft=left;
				$("#editor_'.$name.' .ace_scroller").addClass("ace_scroll-left");				
				$("#editor_'.$name.' .ace_scroller").scrollLeft(0);
			} else {
				$("#editor_'.$name.' .ace_scroller").removeClass("ace_scroll-left");
			}*/
		});
    	    
	</script>
	';


	//testing block
	/*echo '<div id="testing">testing</div>';
	echo "<script>
		function get_test_info(name){
			var r=name+': ';
			var o=$(name);
			var p=o.position();
			var sep=' | ';
			r+=o.scrollLeft()+sep;
			r+=o.css('margin-left')+sep;
			r+=o.css('left')+sep;
			r+=p.left+sep;
			return r+'<br>';
		}
		function testfunc(){
			var r='';
			r+=get_test_info('.ace_content');
			r+=get_test_info('.ace_scroller');
			r+=get_test_info('.ace_editor');
			r+=get_test_info('.ace_text-input');
			r+=get_test_info('.ace_print-margin-layer');
			r+=get_test_info('.ace_marker-layer');
			r+=get_test_info('.ace_text-layer');
			r+=get_test_info('.ace_marker-layer');
			r+=get_test_info('.ace_cursor-layer');
			r+=get_test_info('.ace_scrollbar-h');
			r+=get_test_info('.ace_scrollbar-inner');
			$('#testing').html(r);
		}
		setInterval('testfunc()',1000);
	</script>";*/

}

function add_elem(&$parent,$title='',$url='',$conf=''){
	$elem->title=$title;
	$elem->url=$url;
	$elem->conf=$conf;
	if(empty($parent)) $parent=Array();
	$parent[count($parent)]=$elem;
}

function add_bread(&$bread,$title,$title2,$url,$elements){
	global $lbread;
	$elem->title=$title;
	$elem->title2=$title2;
	$elem->url=$url;
	$elem->elements=$elements;
	$bread[count($bread)]=$elem;
	$lbread=count($bread)-1;
}

function table_bread($tid,$ex2,$owner,$tmodule=0,$tup=0){
	global $db,$id,$id6,$xmodule;
	if($tid==0) return Array();
	if(empty($tmodule)) $tmodule=$id;
	if($ex2==0 && $owner==0){
		if(!empty($_COOKIE["cex".$tmodule])) $ex2=$_COOKIE["cex".$tmodule];
		else {
			getrow($db,"SELECT * FROM ex_module WHERE ex_module=$tmodule",1,"ex_module");
			if(empty($db->Record)) return Array();
			else $ex2=$db->Record["ex_id"];
		}
	}
	if(isset($GLOBALS["id2"])){
		if($xmodule!='mod_col') $id2=$GLOBALS["id2"];
		else $id2=get_tex(0,$ex2,$tid);
		if(empty($id2)){
			$id2=$ex2;
		}
	} else $id2=0;
	$elem=Array();
	$text='';
	if(!empty($owner)){
		if($owner==$id6) $text.=se('view','','','',0,4,5,' height="13" align="absmiddle" ').'<span style="text-decoration: underline;">'.lng('view').'</span>';
		else $text.='<a class="ablack" href="mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$owner.'&amp;id7='.$tid.'">'.se('view','','','',0,4,5,' height="13" align="absmiddle" ').lng('view').'</a>';
	} else {
		//эта часть используется в дальнейшем
		$tmp=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=".$ex2." AND ex_module=$id",1,"ex_group");
		$ex=Array();
		if(!empty($tmp)) foreach($tmp AS $tm) $ex[$tm["ex_table"]]=$tm["ex_ex1"];	
		
		if(empty($id6) && $xmodule=='mod_table' && !empty($ex[$tid]) && $id2==$ex[$tid]) $text.=se('view','',lng('Data view'),'',0,4,5,' height="13" align="absmiddle" ').'<span style="text-decoration: underline;">'.lng('view').'</span>';
		else $text.='<a class="ablack" href="mod_table?id='.$id.'&amp;id2='.$id2.'">'.se('view','',lng('Data view'),'',0,4,5,' height="13" align="absmiddle" ').lng('view').'</a>';
	}
	if(check_tbl($tid,'edit') || check_mod($tmodule,'edit')){
		$text.='<a class="ablack" href="mod_main?id='.$tmodule.'&amp;id2='.$tid.'&amp;action=edit_table_form#edit_table">'.se('edit','',lng('Edit table'),' style="margin-left: 10px;"',0,4,5,' height="11" align="absmiddle" ').lng('edt').'</a>';
		if($xmodule=='mod_col' && $id2=$tid) $text.=se('back_config','',lng('Table variables'),' style="margin-left: 10px;"',0,4,5,' height="13" align="absmiddle" ').'<span style="text-decoration: underline;">'.lng('vars').'</span>';
		else $text.='<a class="ablack" href="mod_col?id='.$tmodule.'&amp;id2='.$tid.'">'.se('back_config','',lng('Table variables'),' style="margin-left: 10px;"',0,4,5,' height="13" align="absmiddle" ').lng('vars').'</a>';
		//add_elem($elem,'Изменить','mod_main?id='.$tmodule.'&amp;id2='.$tid.'&amp;action=edit_table_form#edit_table');
		//add_elem($elem,'Переменные','mod_col?id='.$tmodule.'&amp;id2='.$tid);
	}
	add_elem($elem,'<div align="center" style="padding-top: 1px;">'.$text.'</div>');

	if(!empty($crumb->edit)) echo se('edit',$crumb->edit,'','',1,0,1,' height="10"','margin-left: 4px;');
	if(!empty($crumb->conf)) echo se('back_config',$crumb->conf,'','',1,0,1,' height="12"','margin-left: 3px;');
	$first=true;
	if($owner==0){
		$add1=" AND table_id!=$tid";
		$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id AND table_bold!=2".$add1." ORDER BY table_name",1,"main_table");
		if(!empty($tbl)){
			foreach($tbl AS $tb)if(!empty($ex[$tb["table_id"]]) && check_tbl($tb["table_id"],'view')){
				$edit='';
				if(check_tbl($tb["table_id"],'edit') || check_mod($tb["table_module"],'edit')) $edit='mod_col?id='.$tb["table_module"].'&amp;id2='.$tb["table_id"];
				if($first) add_elem($elem);
				$first=false;
				add_elem($elem,$tb["table_name"],'mod_table?id='.$id.'&amp;id2='.$ex[$tb["table_id"]],$edit);
			}
		}
	} else {
		$tbls=getall($db,"SELECT * FROM table_sub WHERE sub_table1=$tup",1,"table_sub");
		$w='';
		if(!empty($tbls) && count($tbls)>1) foreach($tbls AS $tbl){
			if($w!='') $w.=','; $w.=$tbl["sub_table2"];
		}
		if(!empty($w)){
			$tbls=getall($db,"SELECT * FROM main_table WHERE table_id IN ($w)",1,"main_table");
			if(count($tbls)>1) foreach($tbls AS $tbl) if($tbl["table_id"]!=$tid){
				if($first) add_elem($elem);
				$first=false;
				$edit='';
				if(check_tbl($tbl["table_id"],'edit') || check_mod($tbl["table_module"],'edit')) $edit='mod_col?id='.$tbl["table_module"].'&amp;id2='.$tbl["table_id"];
				add_elem($elem,$tbl["table_name"],'mod_table?id='.$id.'&id2='.$id2.'&id6='.$owner.'&id7='.$tbl["table_id"],$edit);
			}
		}		
	}
	return $elem;
}

function ex_bread($ex,$module){
	global $id,$id2,$db,$xmodule,$id6;
	getrow($db,"SELECT * FROM main_table WHERE table_module=$module AND table_bold=1");
	if(!empty($db->Record["table_id"])) $tbl=$db->Record["table_id"]; else $tbl=0;
	getrow($db,"SELECT * FROM ex_module WHERE ex_id=$ex");
	if(!empty($db->Record["ex_major"])) $tbl=$db->Record["ex_major"];
	if($tbl==0){
		getrow($db,"SELECT * FROM main_table WHERE table_module=$module");
		if(!empty($db->Record["table_id"])) $tbl=$db->Record["table_id"];
	}
	$mtbl=$tbl;
	getrow($db,"SELECT * FROM ex_group WHERE ex_ex2=".$ex." AND ex_module=$id AND ex_table=$tbl",1,"ex_group");
	if(!empty($db->Record["ex_ex1"])) $ex2=$db->Record["ex_ex1"];
	$text='';
	if(empty($ex2)) $ex2=0;
	if(empty($id6) && $xmodule=='mod_table' && $id2==$ex2) $text.=se('view','',lng('Data view'),'',0,4,5,' height="13" align="absmiddle" ').'<span style="text-decoration: underline;">'.lng('view').'</span>';
	else $text.='<a class="ablack" href="mod_table?id='.$id.'&amp;id2='.$ex2.'">'.se('view','',lng('Data view'),'',0,4,5,' height="13" align="absmiddle" ').lng('view').'</a>';
	if(check_ex($ex,'edit')) $text.='<a class="ablack" href="mod_main?id='.$id.'&amp;id2='.$ex.'&amp;action=edit_ex_form#edit_ex">'.se('edit','',lng('Edit section'),' style="margin-left: 10px;"',0,4,5,' height="11" align="absmiddle" ').lng('edt').'</a>';
	if(check_mod($id,'edit')) $text.='<a class="ablack" href="mod_col?id='.$id.'">'.se('back_config','',lng('Module variables'),' style="margin-left: 10px;"',0,4,5,' height="13" align="absmiddle" ').lng('vars').'</a>';
	$elem=Array();
	add_elem($elem,'<div align="center" style="padding-top: 1px;">'.$text.'</div>');

	$tmp=getall($db,"SELECT * FROM ex_group WHERE ex_module=$id",1,"ex_group");
	$ex2=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) $ex2[$tm["ex_ex2"]][$tm["ex_table"]]=$tm["ex_ex1"];	
	
	$exs=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id ORDER BY ex_name",1,"ex_module");	
	if(!empty($exs)) foreach($exs AS $e) if($e["ex_id"]!=$ex && check_ex($e["ex_id"],'view')){
		$tbl=$mtbl;
		if($e["ex_major"]!=0) $tbl=$e["ex_major"];
		if(!empty($ex2[$e["ex_id"]][$tbl])) $idx=$ex2[$e["ex_id"]][$tbl]; else $idx=0;
		if(check_ex($e["ex_id"],'edit')) add_elem($elem,$e["ex_name"],'mod_table?id='.$id.'&amp;id2='.$idx,'mod_main?id='.$id.'&amp;id2='.$e["ex_id"].'&amp;action=edit_ex_form#edit_ex');
		else add_elem($elem,$e["ex_name"],'mod_table?id='.$id.'&amp;id2='.$idx);
	}
	return $elem;
}

function bread_modules(){
	global $user;
	$el=Array();	
	add_elem($el,lng('List of modules'),'modules','');
	if($user->super){
		add_elem($el,lng('Functions'),'parts?type=0','');
		add_elem($el,lng('Showings'),'parts?type=1','');
		add_elem($el,lng('Components'),'parts?type=2','');
		add_elem($el,lng('Forms'),'parts?type=3','');
		add_elem($el,lng('Table templates'),'tbltpl','');
		add_elem($el,lng('Field templates'),'mod_col','');
	}
	return $el;
}

function bread_module($module_id){
	global $m;
}

function create_bread(){
	global $bread,$module,$title,$title_conf,$m,$id6,$id7,$db,$zone_url,$id,$id2,$lbread,$xmodule,$type,$fmod,$group,$user;
	$res_lng=define_lng('breadcrumbs');	
	$title=lng('Control');
	$xmodule=$module;
	if($module=='main'){
		$title=lng('Control panel');
		add_bread($bread,lng('E5 CMS'),'','',Array());
	}
	if($module=='perm'){
		if($id==0){
			if(empty($fmod)){
				$title=lng('Default permissions');
				add_bread($bread,lng('Main'),'',$zone_url,Array());
				add_bread($bread,lng('Access configuration'),'','group',Array());
				add_bread($bread,lng('Default permissions'),'','',Array());
			} else {
				getrow($db,"SELECT * FROM main_module WHERE module_id=$fmod");
				$title=lng('Default permissions').' ('.lng('Module').' "'.$db->Record["module_name"].'")';
				add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$db->Record["module_id"],Array());
				add_bread($bread,lng('Access configuration'),'','group?fmod='.$fmod,Array());	
				add_bread($bread,lng('Default permissions'),'','',Array());
			}
		} else {	
			if(empty($fmod)){
				add_bread($bread,lng('Main'),'',$zone_url,Array());
				add_bread($bread,lng('Access configuration'),'','group',Array());
			} else {
				getrow($db,"SELECT * FROM main_module WHERE module_id=$fmod");
				add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$db->Record["module_id"],Array());
				add_bread($bread,lng('Access configuration'),'','group?fmod='.$fmod,Array());
			}	
			getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$id);
			if($db->Record["auth_type"]==0){
				$title=lng('User permissions').' «'.$db->Record["user_login"].'»';
				$title_conf='group?group=0&amp;id='.$id.'&amp;action=edit';
				if(!empty($fmod)) $title_conf.='&amp;fmod='.$fmod;
				$title_conf.='#edit';
			} else {
				$title=lng('Group permissions').' «'.$db->Record["group_name"].'»';
				$title_conf='group?id='.$id.'&amp;action=edit_form';
				if(!empty($fmod)) $title_conf.='&amp;fmod='.$fmod;
				$title_conf.='#edit';
			}
			add_bread($bread,$title,'','',Array());
		}
	}
	if($module=='group'){
		if(empty($fmod)){
			$title=lng('Access configuration');
			if($user->super) $title_conf='perm?id=0';
			add_bread($bread,lng('Main'),'',$zone_url,Array());
			if(empty($group)){
				if(isset($_GET["group"])){
					$title=lng('Users without a group');
					$title_conf='';
					add_bread($bread,lng('Access configuration'),'','group',Array());
					add_bread($bread,lng('Users without a group'),'','',Array());
				} else add_bread($bread,lng('Access configuration'),'','',Array());
			} else {
				add_bread($bread,lng('Access configuration'),'','group',Array());
				getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$group);
				$title=lng('Users of the group').' «'.$db->Record["group_name"].'»';
				add_bread($bread,lng('Users of the group'),$db->Record["group_name"],'',Array());
			}
		} else {
			getrow($db,"SELECT * FROM main_module WHERE module_id=$fmod");
			$title=lng('Module access configuration').' «'.$db->Record["module_name"].'»';
			add_bread($bread,'Модуль',$db->Record["module_name"],'mod_main?id='.$db->Record["module_id"],Array());
			if(empty($group)){
				add_bread($bread,lng('Access configuration'),'','',Array());
			} else {
				add_bread($bread,lng('Access configuration'),'','group?fmod='.$fmod,Array());
				getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$group);
				$title=lng('Users of the group').' «'.$db->Record["group_name"].'»';
				add_bread($bread,lng('Users of the group'),$db->Record["group_name"],'',Array());
			}
		}
	}
	if($module=='mod_col'){
		if(empty($id) && empty($id2)){
			add_bread($bread,lng('Main'),'',$zone_url,Array());
			add_bread($bread,lng('Modules'),'','modules',bread_modules());
			add_bread($bread,lng('Field templates'),'','mod_col',Array());
			$title=lng('Field templates');
		} else if($id==0 && !empty($id2)){
			add_bread($bread,lng('Main'),'',$zone_url,Array());
			add_bread($bread,lng('Modules'),'','modules',bread_modules());
			add_bread($bread,lng('Table templates'),'','tbltpl',Array());
			getrow($db,"SELECT * FROM main_table WHERE table_id=".$id2);
			$el=Array(); add_elem($el,lng('Edit'),'tbltpl?id='.$id2.'&amp;action=edit_form#edit_form');
			$title=lng('Table').' «'.$db->Record["table_name"].'»';
			add_bread($bread,lng('Table'),$db->Record["table_name"],'',$el);
			add_bread($bread,lng('Variables'),'','',Array());
			$title_conf='tbltpl?id='.$id2.'&amp;action=edit_form#edit_form';
		} else if(!empty($id2)) {
			getrow($db,"SELECT * FROM main_module WHERE module_id=$id");
			add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$db->Record["module_id"],bread_module($db->Record["module_id"]));
			getrow($db,"SELECT * FROM main_table WHERE table_id=".$id2);
			$title=lng('Table').' «'.$db->Record["table_name"].'»';
			$title_conf='mod_main?id='.$id.'&amp;id2='.$id2.'&amp;action=edit_table_form#edit_table';
			add_bread($bread,lng('Table'),$db->Record["table_name"],'',table_bread($db->Record["table_id"],0,0,$db->Record["table_module"],0));
			add_bread($bread,lng('Variables'),'','',Array());
		} else if(!empty($id)) {
			getrow($db,"SELECT * FROM main_module WHERE module_id=$id");
			$title=lng('Module variables').' «'.$db->Record["module_name"].'»';
			add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$db->Record["module_id"],bread_module($db->Record["module_id"]));
			add_bread($bread,lng('Variables'),'','',Array());
		}
	}
	if($module=='parts_param' && isset($type)){
		add_bread($bread,lng('Variables'),'','',Array());
		$el=Array(); add_elem($el,lng('Content'),'mod_part?type='.$type.'&amp;id2='.$id2); add_elem($el,lng('Edit'),'parts?type='.$type.'&amp;part_id='.$id2.'&amp;action=edit_part_form#edit_part');
		getrow($db,"SELECT * FROM main_part WHERE part_id=".$id2);
		$title=lng('Unit variables').' «'.$db->Record["part_name"].'»';
		$title_conf='mod_part?type='.$type.'&amp;id2='.$id2;
		add_bread($bread,lng('Unit'),$db->Record["part_name"],'',$el);
		if($type==0) $pn=lng('Functions');
		if($type==1) $pn=lng('Showings');
		if($type==2) $pn=lng('Components');
		if($type==3) $pn=lng('Forms');
		add_bread($bread,$pn,'','parts?type='.$type,Array());
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		krsort($bread);
	}
	if($module=='parts_param' && !empty($id) && !isset($type) && !empty($id2)){
		add_bread($bread,lng('Variables'),'','',Array());	
		$el=Array(); add_elem($el,lng('Content'),'mod_part?id='.$id.'&amp;id2='.$id2); add_elem($el,lng('Edit'),'mod_main?id='.$id.'&amp;id2='.$id2.'&amp;action=edit_part_form#edit_part');
		getrow($db,"SELECT * FROM main_part WHERE part_id=".$id2);
		$title=lng('Unit variables').' «'.$db->Record["part_name"].'»';
		$title_conf='mod_part?id='.$id.'&amp;id2='.$id2;
		add_bread($bread,lng('Unit'),$db->Record["part_name"],'',$el);
		getrow($db,"SELECT * FROM main_module WHERE module_id=".$id);
		add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$id,bread_module($db->Record["module_id"]));
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		krsort($bread);
	}
	if($module=='mod_part' && (isset($_GET["type"]) || isset($_POST["type"]))){
		add_bread($bread,lng('Content'),'','',Array());
		$el=Array(); add_elem($el,lng('Variables'),'parts_param?type='.$type.'&amp;id2='.$id2); add_elem($el,lng('Edit'),'parts?type='.$type.'&amp;part_id='.$id2.'&amp;action=edit_part_form#edit_part');
		getrow($db,"SELECT * FROM main_part WHERE part_id=".$id2);
		$title=lng('Unit content').' «'.$db->Record["part_name"].'»';
		$title_conf='parts_param?type='.$type.'&amp;id2='.$id2;
		add_bread($bread,lng('Unit'),$db->Record["part_name"],'',$el);
		if($type==0) $pn=lng('Functions');
		if($type==1) $pn=lng('Showings');
		if($type==2) $pn=lng('Components');
		if($type==3) $pn=lng('Forms');		
		add_bread($bread,$pn,'','parts?type='.$type,Array());
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		krsort($bread);
	}
	if($module=='mod_part' && !empty($id) && !isset($_GET["type"]) && !isset($_POST["type"]) && !empty($id2)){
		add_bread($bread,lng('Content'),'','',Array());
		$el=Array(); add_elem($el,lng('Variables'),'parts_param?id='.$id.'&amp;id2='.$id2); add_elem($el,lng('Edit'),'mod_main?id='.$id.'&amp;id2='.$id2.'&amp;action=edit_part_form#edit_part');
		getrow($db,"SELECT * FROM main_part WHERE part_id=".$id2);
		$title=lng('Unit content').' «'.$db->Record["part_name"].'»';
		$title_conf='parts_param?id='.$id.'&amp;id2='.$id2;
		add_bread($bread,lng('Unit'),$db->Record["part_name"],'',$el);
		getrow($db,"SELECT * FROM main_module WHERE module_id=".$id);
		add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$id,bread_module($db->Record["module_id"]));
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		krsort($bread);
	}
	if($module=='tbltpl'){
		$title=lng('Table templates');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,lng('Table templates'),'','',Array());
	}
	if($module=='update'){
		$title=lng('Update');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Update'),'','',Array());
	}
	if($module=='news'){
		$title='Новости';
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,'Новости','','',Array());
	}
	if($module=='deploy'){
		$title=lng('System installation to another server');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('System installation to another server'),'','',Array());
	}
	if($module=='parts'){
		if($type==0) $pn=lng('Functions');
		if($type==1) $pn=lng('Showings');
		if($type==2) $pn=lng('Components');
		if($type==3) $pn=lng('Forms');
		$title=$pn;
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,$pn,'','',Array());
	}
	if($module=='mail'){
		$title=lng('Messages');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Messages'),'','',Array());
	}
	if($module=='settings'){
		$title=lng('Settings');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Settings'),'','',Array());
	}
	if($module=='statistics'){
		$title=lng('Statistics');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Statistics'),'','',Array());
	}
	if($module=='terminal'){
		$title=lng('Termial');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Termial'),'','',Array());
	}
	if($module=='zones'){
		$title=lng('Sites');
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Sites'),'','',Array());
	}
	if($module=='modules'){
		$title='Модули';
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Modules'),'','',bread_modules());
	}
	if($module=='mod_main'){
		if(!empty($_GET["id"]) || !empty($_POST["id"])){
			getrow($db,"SELECT * FROM main_module WHERE module_id=".$id);
			$title='Модуль «'.$db->Record["module_name"].'»';
			add_bread($bread,lng('Main'),'',$zone_url,Array());
			add_bread($bread,lng('Modules'),'','modules',bread_modules());
			add_bread($bread,lng('Module'),$db->Record["module_name"],'',bread_module($db->Record["module_id"]));
		}
	}
	if($module=='mod_update' && !empty($id)){
		getrow($db,"SELECT * FROM main_module WHERE module_id=".$id);
		$title='Обновление модуля «'.$db->Record["module_name"].'»';
		add_bread($bread,lng('Main'),'',$zone_url,Array());
		add_bread($bread,lng('Modules'),'','modules',bread_modules());
		add_bread($bread,lng('Module'),$db->Record["module_name"],'',bread_module($db->Record["module_id"]));
		add_bread($bread,lng('Update'),'','',Array());
	}
	if(!empty($id2) && ($module=='mod_table' || $module=='row_user')){
		getrow($db,"SELECT * FROM ex_table WHERE ex_id=$id2");
		$rtid=$db->Record["ex_table"];
		if(!empty($id7)){
			$tid=$id7;
			$ltid=$tid;
			$owner=$id6; $otid=$tid;
			$ltable=0;
			while($owner!=0){
				getrow($db,"SELECT * FROM row_owner WHERE row_id=$owner");
				$ntable=$db->Record["row_table"];
				$tmp=$db->Record;
				if($ntable==$ltable){
					$owner=$tmp["owner_id"];
					$otid=$tmp["row_table"];
					$ltid=$tmp["row_table"];					
					continue;
				}
				$ltable=$ntable;
				getrow($db,"SELECT * FROM main_table WHERE table_id=$otid");
				$tmp2=$db->Record;
				add_bread($bread,lng('Table'),$db->Record["table_name"],'',table_bread($otid,0,$owner,$db->Record["table_module"],$tmp["row_table"]));
				$bread[$lbread]->view='mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$owner.'&amp;id7='.$tid;
				if(check_tbl($tid,'edit') || check_mod($tmp2["table_module"],'edit')){
					$bread[$lbread]->edit='mod_main?id='.$tmp2["table_module"].'&amp;id2='.$tid.'&amp;action=edit_table_form#edit_table';
					$bread[$lbread]->conf='mod_col?id='.$tmp2["table_module"].'&amp;id2='.$tid;
				}
				$bn=get_basename($owner/*,$otid*/);
				if(strlen($bn)/2>20) $bn=substr_unicode($bn,0,18).'...';
				add_bread($bread,lng('Element'),$bn,'',Array());
				$owner=$tmp["owner_id"];
				$otid=$tmp["row_table"];
				$ltid=$tmp["row_table"];
			}
		} else {
			$tid=$rtid;
			$ltid=$rtid;
		}
		getrow($db,"SELECT * FROM ex_group WHERE ex_ex1=$id2");
		$ex_ex2=$db->Record["ex_ex2"];
		if(!empty($ltid)) getrow($db,"SELECT * FROM main_table WHERE table_id=$ltid");
		$tdb=$db->Record;
		add_bread($bread,lng('Table'),$db->Record["table_name"],'',table_bread($ltid,$ex_ex2,0));
		$db->Record=$tdb;
		$bread[$lbread]->view='mod_table?id='.$id.'&amp;id2='.$id2;
		if(check_tbl($ltid,'edit') || check_mod($db->Record["table_module"],'edit')){
			$bread[$lbread]->edit='mod_main?id='.$db->Record["table_module"].'&amp;id2='.$ltid.'&amp;action=edit_table_form#edit_table';
			$bread[$lbread]->conf='mod_col?id='.$db->Record["table_module"].'&amp;id2='.$ltid;
		}
		if($tid!=$ltid) getrow($db,"SELECT * FROM main_table WHERE table_id=$tid");
		$title=lng('Table').' «'.$db->Record["table_name"].'»';
		if(check_tbl($db->Record["table_id"],'edit',$db->Record["table_module"]) || check_mod($db->Record["table_module"],'edit')){
			if($id6==0) $title_conf=$zone_url.'/mod_col?id='.$db->Record["table_module"].'&amp;id2='.$db->Record["table_id"];
			else {
				getrow($db,"SELECT * FROM row_owner WHERE row_id=".$id6);
				$title_conf=$zone_url.'/mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$id6.'&amp;id4=0&amp;id5='.$db->Record["ro_id"].'&id6='.$db->Record["owner_id"].'&id7='.$db->Record["row_table"].'&action=edit_form&sort=#editform';
			}
		}
		if($id6!=0){
			$xn=get_basename($id6);
			if(strlen($xn)>80) $xn=mb_substr($xn,0,80,'utf-8').'...';
			$title=lng('Element').' «'.$xn.'» <span class="normal">('.$title.')</span>';
		}
		if(!empty($ex_ex2)){
			getrow($db,"SELECT * FROM ex_module WHERE ex_id=".$ex_ex2);
			add_bread($bread,lng('Section'),$db->Record["ex_name"],'',ex_bread($db->Record["ex_id"],$db->Record["ex_module"]));
		}
		getrow($db,"SELECT * FROM main_module WHERE module_id=$id");
		add_bread($bread,lng('Module'),$db->Record["module_name"],'mod_main?id='.$db->Record["module_id"],bread_module($db->Record["module_id"]));
		krsort($bread);
	}
	if($module=='row_user'){
		global $id3;
		add_bread($bread,lng('The owners of element'),get_basename($id3),'',Array());
	}
	define_lng($res_lng);
}

function reset_se(){
	global $se_show;
	$se_show=Array();
}					

$se_sort=Array('search','search2','filter','filter2','require','selectbox','checkbox','bold','url','fastedit','unique','check_on','check_off','deactivate','activate','out','copy','link','edit','refresh','clock','user','usrs','mail','tree','anchor','del','update');
		
function show_se(){
	global $se_show,$se_sort;
	if(empty($se_show)) return '';
	$res='<table cellpadding=0 cellspacing=0 width=100%><tr><td><div align="right" style="padding-top: 10px;"><div style="width: 550px;">';
	foreach($se_show AS $name=>$title) if(strstr($name,'/files/editor/icons/')){
		$res.=' &nbsp;&nbsp; &nbsp;<nobr>'.se($name,'',$title,'',0,0,5).'  '.$title.'</nobr>';
	}
	foreach($se_sort AS $name)if(isset($se_show[$name])){
		$title=$se_show[$name];
		$res.=' &nbsp;&nbsp;&nbsp;<nobr>'.se($name,'',$title,'',0,0,5).'  '.$title.'</nobr>';
	}
	$res.='</div></div></td></tr></table>';
	return $res;
}

function show_help($text){
	return '<div class="help_box"><img src="/files/editor/help.png" align="left" style="margin-right: 5px;">'.$text.'</div>';
}

function si($name,$hspace=5,$vspace=0,$title=''){
	return se($name,'',$title,'',0,$hspace,$vspace);	//simple se
}

function se($name,$url,$title='',$addon='',$is_url=1,$hspace=5,$vspace=0,$addon2='',$addon3=''){	//show element
	global $se_show;
	$res_lng=define_lng('icons');
	if($name=='deactivate') $img='/files/editor/enable.png';
	if($name=='activate') $img='/files/editor/disable.png';
	if($name=='edit') $img='/files/editor/edit.png';
	if($name=='edit2') $img='/files/editor/edit2.png';
	if($name=='copy') $img='/files/editor/copy.png';
	if($name=='del') $img='/files/editor/del.png';
	if($name=='link') $img='/files/editor/link.png';
	if($name=='user') $img='/files/editor/user.png';
	if($name=='usrs') $img='/files/editor/usrs.png';
	if($name=='clock') $img='/files/editor/clock.png';
	if($name=='add') $img='/files/editor/add.png';
	if($name=='add_tpl') $img='/files/editor/add_tpl.png';
	if($name=='useradd') $img='/files/editor/useradd.png';
	if($name=='groupadd') $img='/files/editor/groupadd.png';
	if($name=='tree') $img='/files/editor/tree.png';
	if($name=='tree2') $img='/files/editor/tree2.png';
	if($name=='point') $img='/files/editor/point.png';
	if($name=='mail') $img='/files/editor/mail.png';
	if($name=='protect') $img='/files/editor/deny.gif';
	if($name=='move') $img='/files/editor/move.png';
	if($name=='back') $img='/files/editor/back.png';
	if($name=='back2') $img='/files/editor/back2.png';
	if($name=='out') $img='/files/editor/out.png';
	if($name=='date') $img='/files/editor/date.png';
	if($name=='date2') $img='/files/editor/date2.png';
	if($name=='group') $img='/files/editor/group.png';
	if($name=='invite') $img='/files/editor/invite.png';
	if($name=='help') $img='/files/editor/help.png';
	if($name=='back_table') $img='/files/editor/back_table.png';
	if($name=='back_module') $img='/files/editor/back_module.png';
	if($name=='back_modules') $img='/files/editor/back_modules.png';
	if($name=='back_config') $img='/files/editor/classic/lb1_cfg.png';
	if($name=='select_table') $img='/files/editor/select_table.png';
	if($name=='view') $img='/files/editor/view.png';
	if($name=='view2') $img='/files/editor/view2.png';
	if($name=='all1') $img='/files/editor/all1.png';
	if($name=='all2') $img='/files/editor/all2.png';	
	if($name=='active1') $img='/files/editor/active1.png';
	if($name=='active2') $img='/files/editor/active2.png';
	if($name=='arrow') $img='/files/editor/classic/arrow.png';
	if($name=='deactive1') $img='/files/editor/deactive1.png';
	if($name=='deactive2') $img='/files/editor/deactive2.png';
	if($name=='filter') $img='/files/editor/filter.png';
	if($name=='filter2') $img='/files/editor/filter2.png';
	if($name=='buffer_new') $img='/files/editor/buffer_new.png';
	if($name=='buffer_add') $img='/files/editor/buffer_add.png';
	if($name=='buffer_del') $img='/files/editor/buffer_del.png';
	if($name=='buffer_move') $img='/files/editor/buffer_move.png';
	if($name=='buffer_copy') $img='/files/editor/buffer_copy.png';
	if($name=='buffer_copy2') $img='/files/editor/buffer_copy2.png';
	if($name=='buffer_copy3') $img='/files/editor/buffer_copy3.png';
	if($name=='buffer_clone') $img='/files/editor/buffer_clone.png';
	if($name=='buffer_clear') $img='/files/editor/buffer_clear.png';
	if($name=='fastedit') $img='/files/editor/fastedit.png';
	if($name=='unique') $img='/files/editor/unique.png';
	if($name=='url') $img='/files/editor/urlsplit.png';
	if($name=='anchor') $img='/files/editor/url.png';
	if($name=='bold') $img='/files/editor/bold.png';
	if($name=='require') $img='/files/editor/require.png';
	if($name=='selectbox') $img='/files/editor/selectbox.png';
	if($name=='checkbox') $img='/files/editor/checkbox2.png';
	if($name=='check_on') $img='/files/editor/check_on.png';
	if($name=='check_off') $img='/files/editor/check_off.png';
	if($name=='sub_tables') $img='/files/editor/subtbls.png';
	if($name=='zone') $img='/files/editor/zone.png';
	if($name=='php') $img='/files/editor/classic/code2.png';
	if($name=='code') $img='/files/editor/classic/code.png';
	if($name=='global') $img='/files/editor/global.png';
	if($name=='shell') $img='/files/editor/run.png';
	if($name=='timer') $img='/files/editor/clock.png';
	if($name=='timer2') $img='/files/editor/clock2.png';
	if($name=='search') $img='/files/editor/search.png';
	if($name=='search2') $img='/files/editor/search2.png';
	if($name=='refresh') $img='/files/editor/refresh2.png';
	if($name=='cpu') $img='/files/editor/classic/cpu.png';
	if($name=='update') $img='/files/editor/classic/update.png';
	if($name=='statistics') $img='/files/editor/classic/statistics.png';
	if(!isset($img)){
		$img=$name;
	}	
	if($title==''){
		if($name=='deactivate') $title=lng('Deactivate');
		if($name=='activate') $title=lng('Activate');
		if($name=='edit') $title=lng('Edit');
		if($name=='edit2') $title=lng('Edit');
		if($name=='copy') $title=lng('Duplicate');
		if($name=='del') $title=lng('Delete');
		if($name=='link') $title=lng('Clone');
		if($name=='user') $title=lng('Owner');	
		if($name=='usrs') $title=lng('Owners');
		if($name=='clock') $title=lng('Setting the start time');
		if($name=='add') $title=lng('Add');
		if($name=='add_tpl') $title=lng('Add from template');
		if($name=='protect') $title=lng('Protected');
		if($name=='useradd') $title=lng('Add user');
		if($name=='invite') $title=lng('Invite user');
		if($name=='groupadd') $title=lng('Add group');
		if($name=='tree') $title=lng('Childs');
		if($name=='tree2') $title=lng('Childs');
		if($name=='point') $title=lng('Subcategories');
		if($name=='mail') $title=lng('Write a letter');
		if($name=='move') $title=lng('Move');
		if($name=='back') $title=lng('Go back');
		if($name=='out') $title=lng('Kicked out of the group');
		if($name=='group') $title=lng('Group');
		if($name=='help') $title=lng('Reference information');
		if($name=='view') $title=lng('View');
		if($name=='view2') $title=lng('Hide');
		if($name=='all1') $title=lng('Show all');
		if($name=='active1') $title=lng('Show active');
		if($name=='deactive1') $title=lng('Show inactive');
		if($name=='filter') $title=lng('Filter');
		if($name=='filter2') $title=lng('Filter');
		if($name=='buffer_new') $title=lng('Add to clipboard');
		if($name=='buffer_add') $title=lng('Addition to current clipboard');
		if($name=='buffer_del') $title=lng('Remove from clipboard');
		if($name=='buffer_move') $title=lng('Move from clipboard');
		if($name=='buffer_copy') $title=lng('Copy from clipboard');
		if($name=='buffer_copy2') $title=lng('Copy without subtables');
		if($name=='buffer_copy3') $title=lng('Copy with clone subtables');
		if($name=='buffer_clone') $title=lng('Clone from clipboard');
		if($name=='buffer_clear') $title=lng('Clear clipboard');
		if($name=='fastedit') $title=lng('Available for quick editing');
		if($name=='unique') $title=lng('Unique value');
		if($name=='url') $title=lng('Is used to parse the URL');
		if($name=='anchor') $title=lng('Link');
		if($name=='date') $title=lng('Date');
		if($name=='date2') $title=lng('Holiday');
		if($name=='bold') $title=lng('Is shown in data table');
		if($name=='require') $title=lng('Required field');
		if($name=='selectbox') $title=lng('Single selection (dropdown list)');
		if($name=='checkbox') $title=lng('Multiple selection');
		if($name=='check_on') $title=lng('On');
		if($name=='check_off') $title=lng('Off');
		if($name=='sub_tables') $title=lng('Subtables');
		if($name=='zone') $title=lng('Sites');
		if($name=='php') $title=lng('PHP');
		if($name=='code') $title=lng('E5 template');
		if($name=='global') $title=lng('Global access');
		if($name=='shell') $title=lng('Shell');
		if($name=='timer') $title=lng('Timer');
		if($name=='timer2') $title=lng('Timer on');
		if($name=='search') $title=lng('Index field');
		if($name=='search2') $title=lng('Index field is rebuilt');
		if($name=='refresh') $title=lng('Rebuild index');
		if($name=='cpu') $title=lng('CPU');
		if($name=='update') $title=lng('Update');		
		if($name=='statistics') $title=lng('Statistics attendance');
	}
	$se_show[$name]=$title;
	$res='';
	if($is_url) $res.='<a href="'.$url.'" '.$addon.'>';
	$res.='<img src="'.$GLOBALS["base_root"].$img.'" border="0" title="'.$title.'"';
	if(!$is_url) $res.=' '.$addon;
	$vert='vertical-align: middle;';
	$res.=' hspace="'.$hspace.'" vspace="'.$vspace.'" style="'.$vert.$addon3.'" '.$addon2.'/>';
	if($is_url) $res.='</a>';
	define_lng($res_lng);
	return $res;
}

?>