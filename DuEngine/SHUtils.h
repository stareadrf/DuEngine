#pragma once
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <unordered_set>
#include <opencv\cv.hpp>
#include "HashVec2b.h"
using namespace cv;

#define PI   3.1415926535897932
#define PI_2 1.5707963267948966
#define PI2  6.283185307179586
#define k01 0.2820947918f // Math.sqrt(  1/Math.PI)/2
#define k02 0.4886025119f // Math.sqrt(  3/Math.PI)/2
#define k03 1.0925484306f // Math.sqrt( 15/Math.PI)/2
#define k04 0.3153915652f // Math.sqrt(  5/Math.PI)/4
#define k05 0.5462742153f // Math.sqrt( 15/Math.PI)/4
#define k06 0.5900435860f // Math.sqrt( 70/Math.PI)/8
#define k07 2.8906114210f // Math.sqrt(105/Math.PI)/2
#define k08 0.4570214810f // Math.sqrt( 42/Math.PI)/8
#define k09 0.3731763300f // Math.sqrt(  7/Math.PI)/4
#define k10 1.4453057110f // Math.sqrt(105/Math.PI)/4
#define k11 2.5033429418f // Math.sqrt( 35/Math.PI) * 3 / 4
#define k12 1.7701307698f // Math.sqrt( 70/Math.PI) * 3 / 8
#define k13 0.9461746958f // Math.sqrt(  5/Math.PI) * 3 / 4
#define k14 0.6690465436f // Math.sqrt( 10/Math.PI) * 3 / 8
#define k15 0.1057855469f // Math.sqrt(  1/Math.PI) * 3 / 16
#define k16 0.4730873479f // Math.sqrt(  5/Math.PI) * 3 / 8
#define k17 0.6258357354f // Math.sqrt( 35/Math.PI) * 3 / 16

const int NUM_BANDS = 25;
const int NUM_COEF = (NUM_BANDS * NUM_BANDS);

#define SAL_HEIGHT 32
#define SAL_WIDTH 64
#define USE_WHITE_MASK true

#define VISUALIZATION_ALPHA 0.3
const int NUM_SAMPLES_SQRT = 400;
const int NUM_SAMPLES = (NUM_SAMPLES_SQRT * NUM_SAMPLES_SQRT);
#define USE_TEST_FUNCTION
#define DEBUG_SPHERICAL_METHOD true
#define DEBUG_CARTESIAN_METHOD true
#define DEBUG_OUTPUT_SH_IMAGES true
#define DEBUG_OUTPUT_SALIENCY true
#define DEBUG_INNER_OUTER_CONES true
#define DEBUG_OUTPUT_SALIENCY_VALUES true
#define READ_FROM_SAMPLES true
#define WRITE_SAMPLES true
#define DEBUG_OUTPUT_SAMPLES true

const double INNER_DEGREE = 40;
const double OUTER_SCALE = 2.0;
const double OUTER_DEGREE = INNER_DEGREE * OUTER_SCALE;
const double THETA_INNER_01 = INNER_DEGREE / 2.0 / 180.0;
const double THETA_INNER_RAD = INNER_DEGREE / 180.0 * PI;
const double THETA_OUTER_01 = THETA_INNER_01 * OUTER_SCALE;
const double THETA_OUTER_RAD = THETA_INNER_RAD * OUTER_SCALE;
const double PHI_INNER_01 = INNER_DEGREE / 2.0 / 360.0;
const double PHI_INNER_RAD = INNER_DEGREE / 180.0 * PI;
const double PHI_OUTER_01 = PHI_INNER_01 * OUTER_SCALE;
const double PHI_OUTER_RAD = PHI_INNER_RAD * OUTER_SCALE;
const int THETA_INNER = std::max(1, (int)ceil(THETA_INNER_01 * SAL_HEIGHT));
const int PHI_INNER = std::max(1, (int)ceil(PHI_INNER_01 * SAL_WIDTH));
const int THETA_OUTER = std::max(THETA_INNER + 1, (int)ceil(THETA_OUTER_01 * SAL_HEIGHT));
const int PHI_OUTER = std::max(PHI_INNER + 1, (int)ceil(PHI_OUTER_01 * SAL_WIDTH));

#define VISUALIZATION_ALPHA 0.3
#define LUMINANCE_THRESHOLD 0.1

typedef bool BoolMat[SAL_HEIGHT][SAL_WIDTH];
typedef double SalMat[SAL_HEIGHT][SAL_WIDTH];
typedef double SingleCoefMat[NUM_COEF];
typedef SingleCoefMat CoefMat[SAL_HEIGHT][SAL_WIDTH];
typedef int CntMat[SAL_HEIGHT][SAL_WIDTH];
typedef unordered_set<Vec2b> MaskMap[SAL_HEIGHT][SAL_WIDTH];

const double factorial[100] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600, 6227020800, 87178291200, 1307674368000, 20922789888000,
355687428096000, 6402373705728000, 121645100408832000, 2432902008176640000, 5.109094217170944e19, 1.12400072777760768e21, 2.585201673888497664e22, 6.2044840173323943936e23, 1.5511210043330985984e25
, 4.03291461126605635584e26
, 1.0888869450418352160768e28
, 3.04888344611713860501504e29
, 8.841761993739701954543616e30
, 2.6525285981219105863630848e32
, 8.22283865417792281772556288e33
, 2.6313083693369353016721801216e35
, 8.68331761881188649551819440128e36
, 2.9523279903960414084761860964352e38
, 1.03331479663861449296666513375232e40
, 3.719933267899012174679994481508352e41
, 1.37637530912263450463159795815809024e43
, 5.230226174666011117600072241000742912e44
, 2.03978820811974433586402817399028973568e46
, 8.15915283247897734345611269596115894272e47
, 3.3452526613163807108170062053440751665152e49
, 1.405006117752879898543142606244511569936384e51
, 6.0415263063373835637355132068513997507264512e52
, 2.658271574788448768043625811014615890319638528e54
, 1.1962222086548019456196316149565771506438373376e56
, 5.50262215981208894985030542880025489296165175296e57
, 2.5862324151116818064296435515361197996919763238912e59
, 1.241391559253607267086228904737337503852148635467776e61
, 6.0828186403426756087225216332129537688755283137921024e62
, 3.0414093201713378043612608166064768844377641568960512e64
, 1.551118753287382280224243016469303211063259720016986112e66
, 8.0658175170943878571660636856403766975289505440883277824e67
, 4.274883284060025564298013753389399649690343788366813724672e69
, 2.30843697339241380472092742683027581083278564571807941132288e71
, 1.269640335365827592596510084756651695958032105144943676227584e73
, 7.1099858780486345185404564746372494973649797888116845868744704e74
, 4.052691950487721675568060190543232213498038479622660214518448128e76
, 2.35056133128287857182947491051507468382886231818114292442069991424e78
, 1.3868311854568983573793901972038940634590287677268743254082129494016e80
, 8.3209871127413901442763411832233643807541726063612459524492776964096e81
, 5.075802138772247988008568121766252272260045289880360030994059394809856e83
, 3.14699732603879375256531223549507640880122807972582321921631682478211072e85
, 1.9826083154044400641161467083618981375447736902272686281062795996127297536e87
, 1.268869321858841641034333893351614808028655161745451921988018943752147042304e89
, 8.247650592082470666723170306785496252186258551345437492922123134388955774976e90
, 5.44344939077443064003729240247842752644293064388798874532860126869671081148416e92
, 3.6471110918188685288249859096605464427167635314049524593701628500267962436943872e94
, 2.480035542436830599600990418569171581047399201355367672371710738018221445712183296e96
, 1.71122452428141311372468338881272839092270544893520369393648040923257279754140647424e98
, 1.197857166996989179607278372168909873645893814254642585755536286462800958278984531968e100
, 8.5047858856786231752116764423992601028858460812079623588643076338858868037807901769728e101
, 6.123445837688608686152407038527467274077809178469732898382301496397838498722168927420416e103
, 4.47011546151268434089125713812505111007680070028290501581908009237042210406718331701690368e105
, 3.3078854415193864122595302822125378214568325182093497117061192683541123570097156545925087232e107
, 2.4809140811395398091946477116594033660926243886570122837795894512655842677572867409443815424e109
, 1.885494701666050254987932260861146558230394535379329335672487982961844043495537923117729972224e111
, 1.45183092028285869634070784086308284983740379224208358846781574688061991349156420080065207861248e113
, 1.1324281178206297831457521158732046228731749579488251990048962825668835325234200766245086213177344e115
, 8.94618213078297528685144171539831652069808216779571907213868063227837990693501860533361810841010176e116
, 7.156945704626380229481153372318653216558465734236575257710944505822703925548014884266894486728081408e118
, 5.79712602074736798587973423157810910541235724473162595874586504971639017969389205625618453424974594048e120
, 4.7536433370128417484213820698940494664381329406799332861716093407674399473489914861300713180847916711936e122
, 3.945523969720658651189747118012061057143650340764344627522435752836975156299662933487959194010377087090688e124
, 3.31424013456535326699938757913013128800066628624204948711884603238305913129171686412988572296871675315617792e126
, 2.817104114380550276949479442260611594800566343305742064051019127525600261597959334510402864523409240182751232e128
, 2.42270953836727323817655232034412597152848705524293817508387644967201622497424502767894646349013194655716605952e130
, 2.1077572983795277172136005186993895952297837380613562123229725112146541157275931740806834232364147935047344717824e132
, 1.854826422573984391147968456455462843802209689493993466844215809868895621840281993191001412448045018284166335168512e134
, 1.65079551609084610812169192624536193098396662364965418549135207078331710343785097393999125707876006627290803829997568e136
, 1.485715964481761497309522733620825737885569961284688766942216863704985393094065876545992131370884059645617234469978112e138
, 1.35200152767840296255166568759495142147586866476906677791741734597153670771559994765685283954750449427751168336768008192e140
, 1.2438414054641307255475324325873553077577991715875414356840239582938137710983519518443046123837041347353107486982656753664e142
, 1.156772507081641574759205162306240436214753229576413535186142281213246807121467315215203289516844845303838996289387078090752e144
, 1.08736615665674308027365285256786601004186803580182872307497374434045199869417927630229109214583415458560865651202385340530688e146
, 1.032997848823905926259970209939472709539774634011737286921225057123429398759470312487176537538542446856328223686422660735041536e148
, 9.9167793487094968920957140154189380115818364865126779544437605483849222280909149998768947603700074898207509473896575430563987456e149
, 9.619275968248211985332842594956369871234381391917297615810447731933374561248187549880587917558907265126128418967967816764706783232e151
, 9.42689044888324774562618574305724247380969376407895166349423877729470707002322379888297615920772911982360585058860846042941264756736e153
, 9.3326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976156518286253697920827223758251185210916864e155
};

float sinc(float x);

struct SHSample
{
	Vec2d dSphCoord;
	Vec3d dCartesian;
	Vec2i iCoord;
	Vec2i iSalCoord;
	double coeff[NUM_COEF];
};

// evaluate an Associated Legendre Polynomial P(l,m,x) at x
double P(int l, int m, double x);

double K(int l, int m);

// return a point sample of a Spherical Harmonic basis function
// l is the band, range [0..N]
// m in the range [-l..l]
// theta in the range [0..Pi]
// phi in the range [0..2*Pi]
double SH(int l, int m, double theta, double phi);

double sqr(double x);
// test function
float test_function(float phi, float theta);

float clamp(float x, float a, float b);

float smoothstep(float edge0, float edge1, float x);

Vec3d heatMap(double greyValue);

Vec3b mix(Vec3d a, Vec3d b, double alpha);



void outputResult(vector<Vec3d> &res);