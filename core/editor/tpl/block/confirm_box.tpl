<div class="confirm_box">
	[if $type=='Attension']
		<b>[lng($type)]!</b><br>
	[/if]
	[$title]
	[if $msg]
		<br><br>
		[$msg]
	[/if]
	<form method="post" action="[$action]">
		[$form_body]
		[tree $btns]
			[name=var]
			[tree value]
				<input type="submit" name="[$name]" value="[value]" class="button" style="[if !first?]margin-left: 50px; [/if]width: 100px;">
			[/tree]
			[if !first?]
				<br>
			[/if]
		[/tree]
		[tree $hidden]
			<input type="hidden" name="[var]" value="[value]">
		[/tree]
	</form>
</div>
<br>