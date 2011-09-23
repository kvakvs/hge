
if (window==top) top.location.replace("../index.html?"+window.location.href.substring(window.location.href.lastIndexOf('/')+1));

function setContents(filename) {
 if(top.frames["cont"]) {
	if(top.cont.location.href.indexOf(filename) == -1)
	top.cont.location.replace(filename);
 }
}

function switchContents(filename1, filename2) {
 if(top.frames["cont"]) {
	if(top.cont.location.href.indexOf(filename2) == -1)
	{ top.cont.location.replace(filename2); }
	else
	{ top.cont.location.replace(filename1); }
 }
}