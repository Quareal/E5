<?php

function rotateImage($img, $rotation) {
   $width = imagesx($img);
   $height = imagesy($img);
   switch($rotation) {
     case 90: $newimg= @imagecreatetruecolor($height , $width );break;
     case 180: $newimg= @imagecreatetruecolor($width , $height );break;
     case 270: $newimg= @imagecreatetruecolor($height , $width );break;
     case 0: return $img;break;
     case 360: return $img;break;
   }
   if($newimg) { 
     for($i = 0;$i < $width ; $i++) { 
       for($j = 0;$j < $height ; $j++) {
         $reference = imagecolorat($img,$i,$j);
         switch($rotation) {
           case 90: if(!@imagesetpixel($newimg, ($height - 1) - $j, $i, $reference )){return false;}break;
           case 180: if(!@imagesetpixel($newimg, $width - $i, ($height - 1) - $j, $reference )){return false;}break;
           case 270: if(!@imagesetpixel($newimg, $j, $width - $i, $reference )){return false;}break;
         }
       } 
     } return $newimg; 
   } 
   return false;
 }

function rotate($file,$angle,$type='old'){
	if(empty($file)) return '';
	$ext=strtolower(get_ext($file));
	if(!file_exists(DOCUMENT_ROOT.$file)) return false;
	if($ext=='.jpg' || $ext=='.jpeg') $im=imagecreatefromjpeg(DOCUMENT_ROOT.$file);
	if($ext=='.gif') $im=imagecreatefromgif(DOCUMENT_ROOT.$file);
	if($ext=='.png') $im=imagecreatefrompng(DOCUMENT_ROOT.$file);
	$jpeg_quality=80;
	
	if($type=='old') $tmp=rotateImage($im,$angle);
	else $tmp=imagerotate($im,$angle,0);
	
	if($ext=='.jpg' || $ext=='.jpeg') imagejpeg($tmp,DOCUMENT_ROOT.$file,$jpeg_quality);
	if($ext=='.gif') imagegif($tmp,DOCUMENT_ROOT.$file);
	if($ext=='.png') imagepng($tmp,DOCUMENT_ROOT.$file);
	imagedestroy($tmp);
	imagedestroy($im);
}

function preview($file,$width=800,$height=800,$type=1,$crop=1,$jpeg_quality=80){
	if(empty($file)) return '';
	$ext=strtolower(get_ext($file));
	if(!file_exists(DOCUMENT_ROOT.$file)) return false;
	if($ext=='.jpg' || $ext=='.jpeg') $im=imagecreatefromjpeg(DOCUMENT_ROOT.$file);
	if($ext=='.gif') $im=imagecreatefromgif(DOCUMENT_ROOT.$file);
	if($ext=='.png') $im=imagecreatefrompng(DOCUMENT_ROOT.$file);
	if(empty($im)) return false;
	$w=imagesx($im);
	$h=imagesy($im);
	if($w==0 || $h==0) return false;
	if($type==1){	//по ширине
		$sx=0;
		$sy=0;
		if($w==$width) return '';
		if($w>$width){
			$nw=$width;
			if($w>$width) $nh=round($h/($w/$width)); else $nh=$h;
			$nw2=$nw;$nh2=$nh;
		} else {$nw2=$w; $nh2=$h;$nw=$w; $nh=$h;}
	}		//по высоте
	if($type==2){
		$sx=0;
		$sy=0;
		$nh=$height;
		if($h==$height) return '';
		if($h>$height) $nw=round($w/($h/$height)); else $nw=$w;
		$nw2=$nw;$nh2=$nh;
	}
	if($type==3){	//по ширине и высоте
		$sx=0;
		$sy=0;
		$nw=$width;
		if($w<$width && $h<$height){
			$nw=$w;$nh=$h;
		} else {
			$nh=$h;
			if($w>$width) $nh=round($h/($w/$width));
			if($nh>$height){
				$nh=$height;
				if($h>$height) $nw=round($w/($h/$height));
			}
		}
		$nw2=$nw;$nh2=$nh;
	}
	if($type==4){	//по ширине (высота обрезается)
		//crop   1 - верх, лево; 2 - середина; 3 - низ, право
		$sx=0;
		$sy=0;
		$nw=$width;
		$nh=$height;
		$nw2=$nw;
		$nh2=$nh;
		if($w>$width){
			$nh=round($h/($w/$width));
			if($nh>$height){
				if($crop==1) $sy=0;
				if($crop==2) $sy=round(($height-$nh)/2);
				if($crop==3) $sy=($h-$nh);
			}
		}
		if($nh>$height){$nh2=$height;}
		if($nh2>$nh) $nh2=$nh;
	}
	if($type==5){	//по ширине (высота обрезается)
		$sx=0;
		$sy=0;
		$nh=$height;
		$nw=$width;
		$nw2=$nw;
		$nh2=$nh;
		if($h>$height){
			$nw=round($w/($h/$height));
			if($nw>$width){//
				 $nw=$width;
				if($crop==1) $sx=0;
				if($crop==2) $sx=round(($width-$nw)/2);
				if($crop==3) $sx=($w-$nw);
			}
		}
		if($nw>$width)$nw2=$width;
		if($nw2>$nw) $nw2=$nw;
	}
	if($type==6){	//по высоте (ширина обрезается), но если ширина не дотягивает, то по ширине (высота обрезается)
		$sx=0;
		$sy=0;
		$nw=$width;
		$nh=$height;
		$nw2=$nw;
		$nh2=$nh;
		if($w>$width){
			$nh=round($h/($w/$width));
			if($nh>$height){
				if($crop==1) $sy=0;
				if($crop==2) $sy=round(($height-$nh)/2);
				if($crop==3) $sy=($h-$nh);
			}
		} else {/*раньше этой секции небыло*/
		 	$nw2=$w; $nw=$w;
		 	$nh=$h;
		 	$nh2=$h;
		}
		if($nh>$height){$nh2=$height;}
		if($nh2>$nh){
			$sx=0;
			$sy=0;
			$nh=$height;
			$nw=$width;
			$nw2=$nw;
			$nh2=$nh;
			if($h>$height){
				$nw=round($w/($h/$height));
				if($nw>$width){
					// $nw=$width;
					if($crop==1) $sx=0;
					if($crop==2) $sx=round(($width-$nw)/2);
					if($crop==3) $sx=($w-$nw);
				}
			}
			if($nw>$width)$nw2=$width;
			if($nw2>$nw) $nw2=$nw;
		}
	}
	if($nw2==$w && $nh2==$h) return false;
	$tmp=imagecreatetruecolor($nw2,$nh2);
	if($ext=='.png'){
		imagealphablending($tmp, false);
		imagesavealpha($tmp,true);
		$transparent = imagecolorallocatealpha($tmp, 255, 255, 255, 127);
		imagefilledrectangle($tmp, 0, 0, $nw2, $nh2, $transparent);
	}
	imagecopyresampled($tmp,$im,$sx,$sy,0,0,$nw,$nh,$w,$h);
	if($ext=='.jpg' || $ext=='.jpeg') imagejpeg($tmp,DOCUMENT_ROOT.$file,$jpeg_quality);
	if($ext=='.gif') imagegif($tmp,DOCUMENT_ROOT.$file);
	if($ext=='.png') imagepng($tmp,DOCUMENT_ROOT.$file);
	imagedestroy($tmp);
	imagedestroy($im);
}

?>