/*
 * Copyright (C) 2010 Javeline BV
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

var canvasFactory = require('../lib/o3-canvas')

function drawtocontext(ctx)
{
	ctx.clearRect(0,0,300,300);
    ctx.fillStyle = "rgb(200,0,0)";
    ctx.fillRect (10, 10, 55, 50);

    ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
    ctx.fillRect (30, 30, 55, 50);
    
    ctx.moveTo(20,40);
    ctx.lineTo(260,280);
    ctx.lineTo(20,280);
    ctx.closePath();
	
    ctx.moveTo(10,10);
    ctx.lineTo(300,300);
    ctx.lineTo(10,300);
    ctx.closePath();
    

    ctx.fill();
    ctx.stroke();
	
	ctx.strokeStyle= "rgb(255,128,0)";
	ctx.beginPath();  
	ctx.moveTo(75,25);  
	ctx.quadraticCurveTo(25,25,25,62.5);  
	ctx.quadraticCurveTo(25,100,50,100);  
	ctx.quadraticCurveTo(50,120,30,125);  
	ctx.quadraticCurveTo(60,120,65,100);  
	ctx.quadraticCurveTo(125,100,125,62.5);  
	ctx.quadraticCurveTo(125,25,75,25);  
	ctx.stroke();  

}
/*
function draw() 
{
    var canvas = document.getElementById("canvas");
    var ctx = canvas.getContext("2d");
    drawtocontext(ctx);
}
*/
  
var ctx = canvasFactory(50,50, "argb");

drawtocontext(ctx);
var buf = ctx.pngBuffer();
var check = 
"iVBORw0KGgoAAAANSUhEUgAAADIAAAAyCAYAAAAeP4ixAAADBUlEQVRoge3YXUhTYRzH8e/Z\n"+
"Juh8odRwm6isQozSEE1rZGlTCwwUAiMIrIvSq6C6CKK6iejF64S0QPQmIbKkLsLE6UV5I1Za\n"+
"mBEzokyTaltUinm6EF9It51t543yd7lz9j/Ph/Oc8zz/I4iiKPIPxKD1AOTKKkRvWYXoLasQ\n"+
"veX/g7jdbiXHEXFMUk6qqamhpaWF60CewgMKN5LuiN1ux4KFM5joV3pEYUby1LJhYzd7dYsJ\n"+
"6WG3YtUtJuS3ll4xYb1+9YgJex3RGyaiBVFPmIhXdr1gZNmi6AEj215La4ysm0YtMbLvfrXC\n"+
"KLKN1wKjWD+iNkbRxkpNjOIdoloYVVpdpTEms8QOUY4sYh5xlZmIO02DCZKyIaUAohNVhIA8\n"+
"GJMZUvIhZRv8GIf3j8HrVhkCi5iTdFNGCVaskv63LtFL2fYhCrPfMjCcwc3WbD5MrF04rjoE\n"+
"5jAZHKeTxqCYDNskFUXP2Jj+id7+LC40HMDjMy87TxMIQDyZATGCIFLuGKR8xyAPenO51V7M\n"+
"1LT/4WoGAf+YOPMUR6tcxMZMcampii+e2KC1NP/SuIjpZowxNqSNc762nbHPa6lv3i8JARrf\n"+
"kfnEk4ldOEaOo4E6h0Dr/WJejKSHVEMXkCjjDDequ0kwJ5Pf+I0sT5TEd9liNJ9aAKf33cEg\n"+
"iNQ1n8XgqV2YZqFEc0hl7hMK7K85d/cIM7+Ny54ZqZEd4sMn+dzNqaOcKLvHqdu1fP8Vs/B7\n"+
"OBhZIT300EabpIsnxXmpP9jIxY7DjE5alh0PFSMrJJ54LFjooivgxU3GWa5VN9Ex4MA1nBOg\n"+
"nnSM7FPLhg0nzoCYypJ+vD/NNLoqgtaTilGsZ/eHMUdPsytvmMsPDzErCpLqScEo2rOvhCnK\n"+
"G2bwTRoT3jUh1QuGUbxnX4oxGmfZU/CSzr4tYdULhFGlZ5/HrN80wOTXBN59TA67nj+Maj17\n"+
"qeCk1PGctqepEddbCaPayr4zTcAaHcOVkaGQtx8r5W+MahBn4StcfVspEeemmY+RiGsuxfwB\n"+
"86YSCNDHTtAAAAAASUVORK5CYII="

if(check != buf.toBase64()){
	console.log("TEST FAILED");
	process.exit(-1)
}
console.log("TEST SUCCEEDED");
	

