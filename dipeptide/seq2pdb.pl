#!/usr/bin/perl
use Math::Vector::Real;
use Math::MatrixReal;
$inFile = $ARGV[0];
$outFile = $ARGV[1];
open(SEQ,"$inFile")||die "no sequence file";
$ARGV[0]=~/(\w*).*/;print"Output file: $outFile\n";
open(OUT,">$outFile");
$date=gmtime;
print OUT "REMARK    Created with seq2pdb. $date\n";
@aacid=([[N],[0]],[[CA],[3.004,37.052,24.0321]],[[C],[4.215,36.265,24.5102]],[[O],[4.072,35.224,25.148]]);	#N CA C O
push (@output,[@aacid]);

use constant PI => 3.1415926535897932384626433832795028841971693993751058209;

sub genrot {$rcphi=($_[0]-$_[1])->versor;      #arguments: starting and end point of an axis vector, angle of rotation (negative,plus 180degrees)
$ux=$rcphi->[0];$uy=$rcphi->[1];$uz=$rcphi->[2];$phigrad=$_[2];$phi=(-($phigrad-180))*PI/180;
$ROT=Math::MatrixReal->new_from_rows([[(cos($phi)+($ux*$ux*(1-cos($phi)))),((-sin($phi)*$uz)+($ux*$uy*(1-cos($phi)))),((sin($phi)*$uy)+($ux*$uz*(1-cos($phi))))],[((sin($phi)*$uz)+($uy*$ux*(1-cos($phi)))),(cos($phi)+($uy*$uy*(1-cos($phi)))),((-sin($phi)*$ux)+($uy*$uz*(1-cos($phi))))],[((-sin($phi)*$uy)+($uz*$ux*(1-cos($phi)))),((sin($phi)*$ux)+($uz*$uy*(1-cos($phi)))),(cos($phi)+($uz*$uz*(1-cos($phi))))]]);}


sub rotate {$_[1]=$ROT * $_[0];}	       #arguments:vector, which should be rotated, output vector

sub vec2mat {$_[1]=Math::MatrixReal->new_from_rows([[$_[0]->[0]],[$_[0]->[1]],[$_[0]->[2]]])}

sub mat2vec {$_[1]=V($_[0]->element(1,1),$_[0]->element(2,1),$_[0]->element(3,1));} #transpose row vector into column vector and v.v.args:input vec output vec

sub growN {$CAC=($Rc-$Rca)->versor;$CO=($Ro-$Rc)->versor;$CN=(1.295424887492640*$CAC)-(1.39056829369170*$CO);$rn=$Rc+$CN;}
           #arguments:coordinates C , O , CA of a previous amino acid
sub growCa {$CN=($CN)->versor;
$CAN=($rn-$Rca)->versor;
$NCA=(2.21248354663859*$CAN)-(1.08239413499427*$CN);$rca=$rn+$NCA;}       #--//--

sub growC {$NCA=($NCA)->versor;$rc1=(3.3840917492458*$CAN)-(2.40839975349829*$NCA);
&genrot($rca,$rn,$_[0]);#argument - phi angle
&vec2mat($rc1,$rc1);&rotate($rc1,$rcx);&mat2vec($rcx,$rcx);
$rc=$rca+$rcx;}

sub growO{
$ro1=$Rc-$Ro;				#argument - psi angle
&vec2mat($ro1,$ro1);&rotate($ro1,$rox);&genrot($rc,$rca,$_[0]);&rotate($rox,$rox);&mat2vec($rox,$rox);
$ro=$rc+$rox;}

sub grow{					#arguments:coordinates C , O , CA of a previous amino acid, phi and psi angle
&growN($_[0],$_[1],$_[2]);
&growCa($_[0],$_[1],$_[2]);
&growC($_[3]);
&growO($_[4]);
}

sub pdbprint{$atom="ATOM";$end="  1.00  0.00";$C="C";$a_name=$_[2];print OUT sprintf"%4s%6d%4s%6s%2s%4d%12.3f%8.3f%8.3f%12s\n",$atom,$num,@{$output[$i+1][$_[0]][0]},$a_name,$C,$i+1,$output[$i+1][$_[0]][$_[1]]->[0],$output[$i+1][$_[0]][$_[1]]->[1],$output[$i+1][$_[0]][$_[1]]->[2],$end;$num++;}

sub align_sidechain1 {
$CN_b=$temp[0][1]-$temp[1][1];
$CN_c=$bacid[0][1]-$bacid[1][1];
$axis_CN=$CN_b x $CN_c;
$axis2=$CN_c x $axis_CN;
$cos=atan2($CN_b,$axis2);
$u90=0.5*PI;
$angle_CN=atan2($CN_b,$CN_c);
if($cos<=$u90)
	{$angle=-($angle_CN*180/PI+180)}
else	{$angle=($angle_CN*180/PI+180)};
&genrot($axis_CN,V(0,0,0),$angle);
foreach $iter (@temp){
$vec=${$iter}[1]-$temp[1][1];
&vec2mat($vec,$vec);&rotate($vec,$vec1);
&mat2vec($vec1,$vec1);
${$iter}[1]=$vec1+$temp[1][1];
#print"${$iter}[1]\n $temp[1][1]\n"
}}

sub align_sidechain2 {
$CCA_b=$temp[2][1]-$temp[1][1];
$CCA_c=$bacid[2][1]-$bacid[1][1];
$axis =$bacid[0][1]-$bacid[1][1];
($o1,$CCA_b_n)=$axis->decompose($CCA_b);
($o2,$CCA_c_n)=$axis->decompose($CCA_c);
$axis2=$CCA_c_n x $axis;
$cos=atan2($CCA_b_n,$axis2);
$u90=0.5*PI;
$angle_CN=atan2($CCA_b_n,$CCA_c_n);
if($cos<=$u90)
        {$angle=-($angle_CN*180/PI+180)}
else    {$angle=($angle_CN*180/PI+180)};
&genrot($axis,V(0,0,0),$angle);
foreach $iter (@temp){
$vec=${$iter}[1]-$temp[1][1];
&vec2mat($vec,$vec);&rotate($vec,$vec1);
&mat2vec($vec1,$vec1);
${$iter}[1]=$vec1+$temp[1][1];
#print"${$iter}[1]\n $temp[1][1]\n"
}}

sub align_sidechain 
{&align_sidechain1;&align_sidechain2;}

$num=1;$i=0;
while(! eof SEQ){
	$line =<SEQ>;$line=~/\s*(\S*)\s*(\S*)\s*(\S*)\s*/;$a_name=$1;
	$Rc=V(@{$output[$i][2][1]});                #input coodinates commander
	$Ro=V(@{$output[$i][3][1]});
	$Rca=V(@{$output[$i][1][1]});
    

	&grow($Rc,$Ro,$Rca,-$2,-$3);
	@bacid =([[N],$rn],[[CA],$rca],[[C],$rc],[[O],$ro]);
	
	$acid = $1;
	if (($a_name eq "ACE") or ($a_name eq "NME")) { $acid = "GLY"; };

	open (AA,"PDB/$acid.pdb")||die"can not open PDB of amino acid $acid\n";
	
	while(! eof AA){
		$str=<AA>;$str=~/\S*\s*\S*\s*(\S*)\s*\S*\s*\S*\s*\S*\s*(\S*)\s*(\S*)\s*(\S*).*/;
		push(@temp,[[$1],[$2,$3,$4]])
    }
	$align=$bacid[1][1]-V(@{$temp[1][1]});
	foreach $iter (@temp)
		{${$iter}[1]=V(@{${$iter}[1]})+$align};
	&align_sidechain;
	$prp=@temp;print ". ";
	for ($k=5;$k<=$prp;$k++)
		{push (@bacid,[$temp[$k-1][0],$temp[$k-1][1]])};
	push (@output,[@bacid]);
	
	$min = 1; $max = $prp;
	
	if ($a_name eq "ACE") {$min = $min + 1;};
	if ($a_name eq "NME") {$max = $max - 2;};
	
	for($k=$min;$k<=$max;$k++)
		{&pdbprint($k-1,1,$a_name)};	#1st argument is the number of var in @output, 2nd - number of var in @output submassive
	$i++;undef @temp;
	};
print OUT "TER";print "\n PROCEED \n"
