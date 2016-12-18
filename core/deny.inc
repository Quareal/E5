<?php

	if(empty($_GET["cron"])) header('Content-type: text/html; charset="utf-8"');
	$GLOBALS["zone_deny"]=$GLOBALS["e5uid"];

	$content='';
	if(!empty($action) && $action=='remind'){
		$content.='<h2>Восстановление пароля</h2><br>';
		getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_email='$email'",1,"main_auth");
		if(!empty($db->Record)){
			$content.=user_remind($db->Record["auth_id"]);
		} else $content.='<div>Пользователь с электронным адресом «'.$email.'» не зарегистрирован. Обратитесь к администратору сайта.</div>';
		$content.='<br>';
	} else if(!empty($action) && $action=='change'){
		$content.=user_remind_form(10);
		$content.='<br>';
	} else {
		global $login;
		$content.='<h2>Доступ запрещён</h2>';
		$GLOBALS["sforward"]=str_replace('action=user_exit','action2=user_exit',$GLOBALS["sforward"]);
		if(!empty($auth_err)) $content.='<div class="red">'.$auth_err.'</div>';
		$content.='<div>Если Вы являетесь членом группы администраторов, введите логин и пароль:</div><br>';
		
		if($zone[$za]["zone_safe"]!=0){
			//безопасная форма
			$pwl='';
			$content.='<script type="text/javascript" src="'.$GLOBALS["base_root"].'/files/editor/z-auth.js"></script>';
			$content.='<script type="text/javascript" src="'.$GLOBALS["base_root"].'/files/editor/jstorage.js"></script>';
			$content.='<div class="hidden red" id="wrong_pwl">Пароль не верен</div>
			<table id="records" cellpadding="2" cellspacing="3" class="deny_table">
			<tr><td valign="middle">Логин:</td><td><input type="text" id="login" name="login" value="'.$login.'" class="button"></td></tr>
			<tr><td valign="middle">Пароль:</td><td><input type="password" id="pwl" name="pwl" value="'.$pwl.'" class="button"></td></tr>
			</table>';
			$content.='<br><input type="button" value="Вход" class="button" OnClick="do_auth(\''.$GLOBALS["sforward"].'\');"> &nbsp; <span class="link" onclick="showhide(\'passlost\');">Я забыл пароль</span></form>	';
		} else {
			//обычная форма
			$content.='<form action="'.$GLOBALS["sforward"].'" method="post">
			<input type="hidden" name="action" value="auth">
			<table id="records" cellpadding="2" cellspacing="3" class="deny_table">
			<tr><td valign="middle">Логин:</td><td><input type="text" name="login" value="'.$login.'" class="button"></td></tr>
			<tr><td valign="middle">Пароль:</td><td><input type="password" name="pwl" class="button"></td></tr>
			</table>
			<div><input type="checkbox" class="button" name="remember_me"> Запомнить меня на этом компьютере</div>';
			$content.='<br><input type="submit" value="Вход" class="button"> &nbsp; <span class="link" onclick="showhide(\'passlost\');">Я забыл пароль</span></form>	';
		}
		
		$content.='<div class="hidden" id="passlost">
			<form action="'.$GLOBALS["sforward"].'" method="post">
			<input type="hidden" name="action" value="remind">
			<br>Email: <input type="text" name="email" class="button">
			<br><input type="submit" value="Отправить пароль по почте" class="button"></form>
		</div>';
	}
	
	$vars['content']=$content;
	$vars['title']='Панель входа';
	echo shell_tpl_admin('system/startpage',$vars);

?>