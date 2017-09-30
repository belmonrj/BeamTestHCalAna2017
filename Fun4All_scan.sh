#! /bin/tcsh -f

if ( $#argv < 1 ) then 
	echo "Usage : $0 src_data_file src_dir";
	exit;
endif


set src_data_file = $1;
set src_dir = $2;

set name = `basename $0 .sh`; 



set src_data_file_base = `basename $src_data_file`; 
set words = ($src_data_file_base:as/_/ /);

if ( $#words <3 ) then 
	echo "Usage : $0 src_data_file src_dir; e.g. src_data_file = Prototype_electron_8";
	exit;
endif

set particle = $words[2];
set momentum = $words[3];

echo  "start $PWD $0 $* -> $name ($particle, $momentum)" ;


set q = '"';

echo '########################################'
echo "Fun4All_scan.C"
echo '########################################'


echo '########################################'
echo "Run"
echo '########################################'

echo  root -l -b -q "Fun4All_scan.C()" ;

root -l -b -q "Fun4All_scan.C" ;

cp -fv ./out.root $src_dir/${src_data_file}.root

date ;
echo "end" ;
