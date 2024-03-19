$fn=50;

WALL_THICKNESS = 2;
BOX_LENGTH = 64; // should be 60 im v2
BOX_WIDTH = 34;
BOX_HEIGHT = 25;
EAR_DISTANCE = 10;
FOOT_HEIGHT=3;
VENT_RADIUS=2;

module wisblock_foot() {
    difference() {
        linear_extrude(FOOT_HEIGHT) circle(5/2);
        linear_extrude(FOOT_HEIGHT) circle(2.2/2);
    }
}

module ear() {
    linear_extrude(WALL_THICKNESS) difference() {
        union() {
            translate([4,4,0]) circle(4);
            square([4,8]);
        }
        translate([4,4,0]) circle(1.5);
    }
}

module ears() {
    translate([10,0,0]) rotate([0,0,-90]) ear();
    translate([BOX_LENGTH+2*WALL_THICKNESS-10-8,0,0]) rotate([0,0,-90]) ear();
    translate([10+8,BOX_WIDTH+2*WALL_THICKNESS,0]) rotate([0,0,90]) ear();
    translate([BOX_LENGTH+2*WALL_THICKNESS-10,BOX_WIDTH+2*WALL_THICKNESS,0]) rotate([0,0,90]) ear();
}    

module base() {
    
    union() {
        
        // Base
        linear_extrude(WALL_THICKNESS) 
            difference() {
                square([BOX_LENGTH+2*WALL_THICKNESS, BOX_WIDTH+2*WALL_THICKNESS]);
                translate([12+WALL_THICKNESS,BOX_WIDTH+WALL_THICKNESS-4,0]) circle(3.5);
            }
        
        // Wisblock feet
        translate([WALL_THICKNESS+1,WALL_THICKNESS,WALL_THICKNESS]) union() {
            translate([4,4,0]) wisblock_foot();
            translate([56,4,0]) wisblock_foot();
            translate([4,26,0]) wisblock_foot();
            translate([58,28,0]) wisblock_foot();
        }
        
        // Ears
        ears();
        
    }
    
}

module box(height) {

    union() {
        
        difference() {
            linear_extrude(WALL_THICKNESS+height) 
                square([BOX_LENGTH+2*WALL_THICKNESS, BOX_WIDTH+2*WALL_THICKNESS]);
            translate([WALL_THICKNESS,WALL_THICKNESS,WALL_THICKNESS])
                linear_extrude(WALL_THICKNESS+height) 
                    square([BOX_LENGTH, BOX_WIDTH]);
        }

        translate([0,0,height]) ears();

    }
}

module battery_box() {
    box(10);
}

module usb_hole(hole_height=6, hole_width=20) {
    hole_depth=WALL_THICKNESS+4;
    translate([-(hole_width-hole_height)/2,0,-hole_depth/2]) linear_extrude(hole_depth) hull() {
        circle(hole_height/2);
        translate([hole_width-hole_height,0,0]) circle(hole_height/2);
    }
}

module sma_hole() {
    translate([0,0,-3]) linear_extrude(6) 
        difference() {
            circle(3.2);
            translate([-3,2.7,0]) square([6,6], center=false);
        }
}

module vents() {
    max_x = floor(BOX_LENGTH / VENT_RADIUS / 3);
    max_y = floor(BOX_WIDTH / VENT_RADIUS / 3);
    shift = 0;
    for (x=[0:1:max_x]) {
        shift = 1 - shift;
        for (y=[0:1:max_y]) {
            translate([x*(VENT_RADIUS * 3)+2*VENT_RADIUS,y*(VENT_RADIUS * 3)+2*VENT_RADIUS,0]) circle(VENT_RADIUS);
        }
    }
}

module wisblock_box() {
    height=18;
    difference() {
        box(height);
        //translate([-1,WALL_THICKNESS+BOX_WIDTH/2,height-(WALL_THICKNESS+FOOT_HEIGHT+2)+3]) rotate([90,0,90]) usb_hole();
        //translate([BOX_LENGTH/2+WALL_THICKNESS+8,-1,height-(WALL_THICKNESS+FOOT_HEIGHT+2)]) rotate([90,0,0]) sma_hole();
        linear_extrude(WALL_THICKNESS+1) vents();
    }
}

//base();
//battery_box();
wisblock_box();        
//vents();