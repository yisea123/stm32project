# process image handler
#
#
#

package require Tk
package require Ttk
# Monitor a directory for all events, and cause some to trigger
# https://sourceforge.net/projects/tcl-inotify/?source=typ_redirect
# reg tcl84-dev linux package
package require inotify

set num 0


# font for description Pane
set font ""

proc help {} {}


# Variable holding process io values like digin(port,bit), or anin(channel)
global digout
global digin
global anin
global anout

set w .ttknote
catch {destroy $w}


# Inotify handler for digout files (ports)
proc digouthandler { fd } {
    global digout
    variable num

    #puts "____________________________________________"
    #puts "Invocation:$num"
    #puts "Instance: $fd"
    #puts "Remaining:[watchdigout queue]"
    #puts "Inotify Info:[inotify info]"
    #puts "Watch Info:[watchdigout info]"
    set events [watchdigout read]
    #puts "Events (raw):$events"
    # puts "Nr Events [llength $events]"

    # In single¿threaded scripts, reading this buffer always gives the data that caused to invoke the handler
    foreach event $events {
        #puts "Event:$event"
        # puts "Object: [lindex $event 7], Flags: [lindex $event 3]"
        set f [lindex $event 7]

        if [string equal [lindex $event 3] "w"] {
	  # was written to
	    set val [exec cat digout/$f]
	    # puts "Value port $f: \"$val\" [string length $val]" 

	    if [string is integer $val] {
		for {set i 0} {$i < 8} {incr i} {
		    if { "$val" & (1 << $i) } {
			# puts -nonewline "1 "
			set digout($f,$i) 1
		    } else {
			# puts -nonewline "0 "
			set digout($f,$i) 0
		    }
		}
	    }
        }
    }
    incr num
}

# Inotify handler for anout files (ports)
proc anouthandler { fd } {
    global anout

    set events [watchanout read]
    #puts "Events (raw):$events"
    # puts "Nr Events [llength $events]"

    # In single¿threaded scripts, reading this buffer always gives the data that caused to invoke the handler
    foreach event $events {
        #puts "Event:$event"
        #puts "Object: [lindex $event 7], Flags: [lindex $event 3]"
        set f [lindex $event 7]

        if [string equal [lindex $event 3] "w"] {
	  # was written to
	    set val [exec cat anout/$f]
	    # puts "Value port $f: $val" 
		#
		set anout($f) $val

        }
    }
}



toplevel $w
wm title $w "CiA 401 process image handler"
wm iconname $w "ttknote"
#positionWindow $w

ttk::frame $w.f
pack $w.f -fill both -expand 1
set w $w.f


## Make the notebook and set up Ctrl+Tab traversal
ttk::notebook $w.note
pack $w.note -fill both -expand 1 -padx 2 -pady 3
ttk::notebook::enableTraversal $w.note

## Popuplate the first pane
ttk::frame $w.note.msg
ttk::label $w.note.msg.m -font $font -wraplength 4i -justify left -anchor n -text \
  "This is the graphical interface for the CANopen CiA 401 module. Each Pane contains one of the four basic process IO groups. To switch between the tabs using Ctrl+Tab when the notebook page heading itself is selected.\
  \nData is exchanged using files in different directories like digin/1, digin/2 ..., digout/1 .."

$w.note add $w.note.msg -text "Description" -underline 0 -padding 2
grid $w.note.msg.m - -sticky new -pady 2
#grid $w.note.msg.b $w.note.msg.l -pady {2 4}
grid rowconfigure $w.note.msg 1 -weight 1
grid columnconfigure $w.note.msg {0 1} -weight 1 -uniform 1

## Populate the digin pane
ttk::frame $w.note.digin
$w.note add $w.note.digin -text "Dig In"

## Popuplate the digout pane
ttk::frame $w.note.digout
$w.note add $w.note.digout -text "Dig Out"


proc populateDigIn {w} {
global digin

    # find out the number of ports 
    set allfiles [glob {digin/*}]
    # match pattern and count this number
    # only digits until end of word \M
    set ports [regsub -all {digin/(\d+\M)} $allfiles {\0} tmp]
    # arrange file events
    
    # for all ports found, 
    for {set j 1} {$j <= $ports} {incr j} {

	ttk::labelframe $w.p$j -text "digin port $j"

	# use checkboxes for each bit to display the value
	#pack configure $w -anchor  n 
	for {set i 7} {$i >= 0} {incr i -1} {
	    checkbutton $w.p${j}.$i	-text $i -relief flat \
	    	-variable digin($j,$i) \
		-command "writeDigIn $j"
	    pack $w.p${j}.$i -anchor nw -side left

	}
	pack $w.p$j -pady 10
    }
}
# write the value to the file digin/channel
proc writeDigIn {channel} {
    global digin
    set val 0
    for {set i 0} {$i < 8} {incr i} {
	set val [expr $val + ($digin($channel,$i) * (1 << $i))]
    }

    #puts "changed digin $channel to $val"
    exec /bin/echo $val > digin/$channel
}

proc populateDigOut {w} {
global digout
    # find out the number of ports 
    set allfiles [glob {digout/*}]
    # match pattern and count this number
    # only digits until end of word \M
    set ports [regsub -all {digout/(\d+\M)} $allfiles {\0} tmp]
    # arrange file events
    
    # for all ports found, 
    for {set j 1} {$j <= $ports} {incr j} {

	ttk::labelframe $w.p$j -text "digout port $j"

	# use checkboxes for each bit to display the value
	#pack configure $w -anchor  n 
	for {set i 7} {$i >= 0} {incr i -1} {
	    checkbutton $w.p${j}.$i	-text $i -relief flat -variable digout($j,$i)
	    pack $w.p${j}.$i -anchor nw -side left

	}
	pack $w.p$j -pady 10
    }

}

populateDigIn  $w.note.digin
populateDigOut $w.note.digout

set id1 [inotify create "watchdigout" "::digouthandler"]
watchdigout add digout {*}

set id2 [inotify create "watchanout" "::anouthandler"]
watchanout add anout {*}



## Populate the anin pane
ttk::frame $w.note.anin
$w.note add $w.note.anin -text "An In"

## Populate the anout pane
ttk::frame $w.note.anout
$w.note add $w.note.anout -text "An Out"
#
#

proc populateAnIn {w} {
global anin

    # find out the number of ports 
    set allfiles [glob {anin/*}]
    # match pattern and count this number
    # only digits until end of word \M
    set ports [regsub -all {anin/(\d+\M)} $allfiles {\0} tmp]
    # arrange file events
    # puts "an in ports $ports"
    
    # for all ports found, 
    for {set j 1} {$j <= $ports} {incr j} {

	ttk::labelframe $w.p$j -text "anin port $j"
	scale $w.p$j.scale -orient horizontal -length 284 -from -100 -to 100 \
		-length 10c \
	     -sliderlength .5c \
	     -tickinterval 20 \
	     -bigincrement 0 \
	     -variable anin($j) \
	     -command "writeAnIn $j"
	# -command "setWidth 

	pack $w.p$j.scale
	pack $w.p$j -pady 5
    }
}

# write the value to the file anin/channel
proc writeAnIn {channel val} {
    global anin
    # puts "changed anin $channel to $val
    set val [expr $val * 320]
    exec /bin/echo $val > anin/$channel
}

populateAnIn  $w.note.anin

proc populateAnOut {w} {
global anout

    # find out the number of ports 
    set allfiles [glob {anout/*}]
    # match pattern and count this number
    # only digits until end of word \M
    set ports [regsub -all {anout/(\d+\M)} $allfiles {\0} tmp]
    # arrange file events
    # puts "an out ports $ports"
    
    # for all ports found, 
    for {set j 1} {$j <= $ports} {incr j} {

	ttk::labelframe $w.p$j -text "anout port $j"
	scale $w.p$j.scale -orient horizontal -from -100 -to 100 \
		-length 10c \
	    -sliderlength .5c \
	    -tickinterval 20 \
	    -bigincrement 0 \
	    -variable anout($j) \

	pack $w.p$j.scale
	pack $w.p$j -pady 5
    }
}
populateAnOut $w.note.anout

# start wit dign selected
$w.note select $w.note.digin

