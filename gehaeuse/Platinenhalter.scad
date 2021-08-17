$fn=50;
difference(){
    union(){
        cylinder(h=7,d=10);
        translate([0,-57,0])cylinder(h=2,d=10);
        translate([78,0,0])cylinder(h=2,d=10);
        translate([0,-5,0])cube([78,10,2]);
        translate([5,-78,0])cube([78,78,2]);
        translate([0,-78,0])cylinder(h=2,d=10);
        translate([78,-78,0])cylinder(h=2,d=10);
        translate([0,-83,0])cube([78,5,2]);
        translate([0,-83,0])cube([78,5,2]);
        translate([-5,-78,0])cube([11,21,2]);
        translate([0,-62,0])cube([11,10,2]);
        translate([0,-72,0])cylinder(h=7,d=10);
        translate([26,-78,0]) cylinder(h=7,d=8);
        translate([67,-78,0]) cylinder(h=7,d=8);
        translate([46,-13,0]) cylinder(h=7,d=8);
    }
    translate([0,0,-.1]) cylinder(h=8,d=4);
    translate([78,-13,-.1]) cylinder(h=8,d=4);
    translate([26,-78,-.1]) cylinder(h=8,d=2.8);
    translate([67,-78,-.1]) cylinder(h=8,d=2.8);
    translate([15,-70,-0.1])cube([58,50,3]);
    translate([0,-72,-0.1])cylinder(h=8,d=2.8);
    translate([0,-57,-0.1])cylinder(h=3,d=4);
}