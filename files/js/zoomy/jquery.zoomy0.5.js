/*
 * 	Zoomy Zoom - jQuery plugin
 *	written by Jacob Lowe	
 *	http://redeyeops.com/plugins/zoomy
 *
 *	Copyright (c) 2010 Jacob Lowe (http://redeyeoperations.com)
 *	Dual licensed under the MIT (MIT-LICENSE.txt)
 *	and GPL (GPL-LICENSE.txt) licenses.
 *
 *	Built for jQuery library
 *	http://jquery.com
 */

jQuery.fn.zoomy = function(options){
    
    var defaults =  {
        zoomSize : 200,
        clickable : false,
        round: true,
        glare: true
    };
    
    var options = $.extend(defaults, options);
    
    var ele = $(this);
    if(ele.size() > 1){
    ele.each(function(){
        addZoomy($(this));   
    })
    }else{
        addZoomy(ele)
    }
    
    
    function addZoomy(ele){
        var h = ele.find('img').css('height'),
            w = ele.find('img').css('width'),
            image = ele.attr('href');
        
        ele.css({'position': 'relative'}).append('<div class="zoomy" style="width:1px;overflow:hidden"><img /></div>');
        var zoom = ele.find('.zoomy');
        
        zoomParams(ele, zoom);
        
        ele.hover(function(){
            if(zoom.attr('id') != 'brokeZoomy'){
                zoom.show(100);
                if(zoom.find('img').length){
                    loadImage(ele, image, zoom);
                    setTimeout(function(){
                        if(!zoom.find('img').length){
                            resetZoom(ele, zoom);
                            startZoom(ele, zoom);
                        }
                    },150)
                }else{
                    resetZoom(ele, zoom);
                    startZoom(ele, zoom);
                }
                ele.find('img:first').stop().animate({opacity: .5},100);
            }else{
                ele.find('img:first').stop().animate({opacity: 1},100);  
            }
        },function(){
            if(zoom.attr('id') != 'brokeZoomy'){
                if(zoom.find('img').length){
                }else{
                setTimeout(function(){
                    zoom.hide();
                },100);
                ele.find('img:first').stop().animate({opacity: 1},100);
                }
            }
        }).click(function(){
            if(options.clickable === false){
             return false;
            }
        });
    };
    function loadImage(ele, image, zoom){
        var y = ele.children('img').height(),
            x = ele.children('img').width(),
            zS = options.zoomSize/2;
            
        
        if(zoom.find('img').attr('src') != image){        
        zoom.css({top: y/2-zS, left: x/2-zS}).find('img').attr('src', image).load(function(){
            var h = zoom.find('img').height(),
                w = zoom.find('img').width();
            ele.attr({'x': w, 'y': h});
            if(options.glare === true){
                zoom.html('<span></span>').css({'background-image': 'url('+image+')'});
                setTimeout(function(){
                    setGlare(zoom);
                },100)
            }else{
                zoom.html('').css({'background-image': 'url('+image+')'});  
            }
            startZoom(ele, zoom);
        })
        }
    }
    
    function startZoom(ele, zoom){
        //fix varible to apropiate names
        //mouse position X,Y
        //ratio X, Y
        //
        var l = zoom.offset()
            ele.mousemove(function(e){
                var zoomImgX = parseInt(ele.attr('x')),
                    zoomImgY = parseInt(ele.attr('y')),
                    tnImgX = ele.width(),
                    tnImgY = ele.height();
                    zoomSize = options.zoomSize;
   		var vbool=false;
  	        if(zoomImgX<zoomSize){ vbool=true; zoomSize=zoomImgX; options.zoomSize=zoomImgX; }
                var halfSize = zoomSize/2,
                    posX = e.pageX-l.left-halfSize,
                    posY = e.pageY-l.top-halfSize,
                    ratioX = tnImgX/zoomImgX,
                    ratioY = tnImgY/zoomImgY,
                    leftX = Math.round((e.pageX-l.left)/ratioX)-halfSize,
                    topY = Math.round((e.pageY-l.top)/ratioY)-halfSize,
                    stop = Math.round(halfSize-(halfSize*ratioX)),
                    rightStop = (tnImgX-zoomSize)+stop;
                    bottomStop = (tnImgY-zoomSize)+stop;
                //positioning and restictions on zoom object
                //-----------------------------------------------------------------------
                //if the zoom object is in the middle
		//alert('leftX: '+leftX+'; topY: '+topY+ ', posX: '+posX+', posY: '+posY);

		//alert((zoomImgX-zoomSize));
		var zix=zoomImgX-zoomSize;
		var ziy=zoomImgY-zoomSize;
		//if(zix<0) zix=/*-zix*/'center'; else zix=zix+'px';
		if(zix<0) zix=-zix;
		if(ziy<0) ziy=-ziy;
		//if(topY<0) topY=0;
		//if(topX<0) topX=0;
		//if(leftX<0) leftX=0;
		//if(leftY<0) leftY=0;
		//if(posX<0) posX=0;
		//if(posY<0) posY=0;
		//if(ziy<0) ziy=/*-ziy*/'middle'; else ziy=ziy+'px';
		//(zoomImgX-zoomSize)+'px '+'0px', left: rightStop, top: -stop}
		//(zoomImgY-zoomSize)+'px', left: -stop, top:  bottomStop

                if(-stop<= posX && -stop<=posY && rightStop>=posX && bottomStop>=posY){
                    zoom.show().css({backgroundPosition: '-'+leftX+'px '+'-'+topY+ 'px', left: posX, top: posY});
                //if zoom object is on the far left wall
                }else if(-stop>= posX){
                    if(-stop<= posY && bottomStop>= posY){
			if(-stop==posX || !vbool)zoom.show().css({backgroundPosition: '0px '+'-'+topY+ 'px', left: -stop, top: posY});
			else zoom.show().css({backgroundPosition: 'center '+'-'+topY+ 'px', left: -stop, top: posY});
			//if(zix=='center') zoom.show().css({backgroundPosition: 'center '+'-'+topY+ 'px', left: -stop, top: posY});
                        //else zoom.show().css({backgroundPosition: '0px '+'-'+topY+ 'px', left: -stop, top: posY});
                    //if zoom object is in top left corner
                    }else if(-stop>= posY){
                        zoom.show().css({backgroundPosition: '0px 0px', left: -stop, top: -stop});
                    //if zoom object is in bottom left corner
                    }else if(bottomStop<= posY){
                        zoom.show().css({backgroundPosition: '0px -'+(ziy)+'px', left: -stop, top:  bottomStop});
                    }
                //if zoom object is on the top wall
                }else if(-stop>+ posY){
                    if(rightStop>posX){
                        zoom.show().css({backgroundPosition: '-'+leftX+'px '+'0px', left: posX, top: -stop});
                    //if zoom object is in top right corner
                    }else{
                        zoom.show().css({backgroundPosition: '-'+(zix)+'px '+'0px', left: rightStop, top: -stop});
                    }
                //if zoom object is on the far right wall
                }else if(rightStop<=posX){
                    if(bottomStop>posY){
                        zoom.show().css({backgroundPosition: '-'+(zix)+'px '+'-'+topY+ 'px', left: rightStop, top: posY});
                    //if zoom object is in bottom right corner
                    }else{
                        zoom.show().css({backgroundPosition: '-'+(zix)+'px '+'-'+(ziy)+ 'px', left: rightStop, top: bottomStop});
                    }
                //if zoom object is on the bottom wall
                }else if(bottomStop<=posY){
                    zoom.show().css({backgroundPosition: '-'+leftX+'px '+'-'+(ziy)+'px', left: posX, top: bottomStop});
                }
            });
            
    }
    
    
    function resetZoom(ele, zoom){
        var img = ele.children('img')
            h = img.height(),
            w = img.width();
        zoom.css({backgroundPosition: 'center', left: '0px', top: '0px'}).show().parent('a').css({height: h, width: w});
    }
    
    function zoomParams(ele, zoom){
        var img = ele.children('img');
        
        
        if(options.round === true){
            zoom.css({height: options.zoomSize ,width: options.zoomSize, '-webkit-border-radius': options.zoomSize/2+'px', '-moz-border-radius': options.zoomSize/2+'px', 'border-radius': options.zoomSize/2+'px'});
        }else{
            zoom.css({height: options.zoomSize ,width: options.zoomSize}).children('span').css({height: options.zoomSize-20,width: options.zoomSize-20});    
        }
        if(img.css('float') == 'left'){
            if(ele.children('img').css('margin') == '0px'){
                ele.css({'margin': '0px', 'float': 'left'});
            }else{
                var margin = ele.children('img').css('margin-top');
                img.css('margin', '0px');
                ele.css({'margin': margin, 'float': 'left'});
            }
            
        }else if(img.css('float') == 'right'){
            if(ele.children('img').css('margin') == '0px'){
                ele.css({'margin': '0px', 'float': 'right'});
            }else{
                var margin = ele.children('img').css('margin-top');
                img.css('margin', '0px');
                ele.css({'margin': margin, 'float': 'right'});
            }
            
        }else{
            if(ele.parent('*').css('text-align') == 'center'){
                if(ele.children('img').css('margin') == '0px'){
                    ele.css({'margin': '0px auto', 'display': 'block'});
                }else{
                    var margin = ele.children('img').css('margin-top');
                    img.css('margin', '0px');
                    ele.css({'margin': margin+' auto', 'display': 'block'});
                }
            }else{
                    ele.css({'display': 'block'})
            }
        }
        img.load(function(){
            setTimeout(function(){
                var h = img.height(),
                w = img.width();
                ele.css({'display': 'block', height: h, width: w, 'cursor': 'normal'});
            },200)
        });
 
    }
    
    function setGlare(zoom){
        var glare = zoom.children('span');
        if(options.round === true){
            glare.css({height: options.zoomSize-20,width: options.zoomSize-20, '-webkit-border-radius': options.zoomSize/2+'px', '-moz-border-radius': options.zoomSize/2+'px', 'border-radius': options.zoomSize/2+'px'})
        }else{
            glare.css({height: options.zoomSize-20,width: options.zoomSize-20})
        }
        
    }
}