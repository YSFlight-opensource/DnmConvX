#pragma once
#ifndef _DNMCONVX_
#define _DNMCONVX_
#include<array>		// std::tr1::array<>
#include<string>	// string
#include<sstream>	// istringstream stringstream
#include<iostream>	// cout endl getchar() ostream
#include<iomanip>	// setprecision() hex setw() setfill() right
#include<fstream>	// ifstream ofstream
#include<vector>	// vector<>
#include<map>		// map<> make_pair()
#include<math.h>	// sqrtf() cos() sin()
#include<functional>
#include<regex>
//#include<valarray>
// using std::cout;
// using std::endl;
// using std::getchar;
// using std::string;
// using std::ostream;
// using std::fixed;
// using std::setprecision;
// using std::hex;
// using std::setw;
// using std::setfill;
// using std::right;
// using std::ifstream;
// using std::ofstream;
// using std::ios_base;
// using std::atoi;
// using std::atof;
// using std::vector;
// using std::map;
// using std::queue;
// using std::make_pair;
// using std::stringstream;
// using std::istringstream;
// using std::ostringstream;
// using std::valarray;
using std::tr1::array;

using namespace std;

typedef const char*		cstr;
typedef char			c08;
typedef unsigned char	u08;

typedef long int		i32;
typedef short int		i16;
typedef unsigned long	u32;
typedef unsigned short	u16;

typedef float			f32;
typedef array<i32,3>	ai3;
typedef array<f32,3>	af3;
typedef array<f32,9>	af9;
typedef vector<string>	vs;
typedef vector<i32>		vi;
typedef vector<u16>		vu;
typedef vector<f32>		vf;
typedef map<u16,u16>	uu;

typedef vs::const_iterator	it_vs_;
typedef vs::iterator		it_vs;
typedef vi::const_iterator	it_vi_;
typedef vi::iterator		it_vi;
typedef vu::const_iterator	it_vu_;
typedef vu::iterator		it_vu;
typedef vf::const_iterator	it_vf_;
typedef vf::iterator		it_vf;
typedef uu::const_iterator	it_uu_;
typedef uu::iterator		it_uu;

#pragma warning(push)	// start of disableSpecificWarnings
#pragma warning(disable:4258)

// equivalent trick for(auto&it:container) from c++14
#define each(i,c) for(i i=(c).begin();(c).end()!=i;++i)

#define all(c) (c).begin(),(c).end()

#define RETURN_CONST_C_STR(ss)\
	static string st;\
	st=ss.str();\
	return st.c_str();

#define FLOAT_PRECISION(q)\
	fixed<<setprecision(q);

#define NESTED_MESH false
enum E_ERROR{
	E_NotError,
	E_IniPath,
	E_IniRead,
	E_MemoryError,
	E_DnmRead,
	E_XWrite,
	E_Error
};
// template<typename T>
// ostream&operator<<(ostream& os,const vector<T>&v){
// 	for(auto&el:v)os<<el<<'~';
// 	return os;
// }
typedef enum E_flib{NOT=false,YES=true}flib;
inline flib&operator++(flib&b)		{return b=YES;}				// prefix ++
inline flib operator++(flib&b,int)	{flib t=b;++b;return t;}	// postfix ++
inline flib&operator--(flib&b)		{return b=NOT;}				// prefix --
inline flib operator--(flib&b,int)	{flib t=b;--b;return t;}	// postfix --
// inline flib&operator=(flib&f,const bool&b){return f=b?YES:NOT}

class CDnmConvX{
public:
	CDnmConvX(void);
// 	~CDnmConvX(void);
	CDnmConvX&operator<<(istringstream&iss);
	operator cstr();
	u16 outputToXFile(cstr outPath="");
	u16 inputDnmFile(cstr inPath);
	u16 inputIniFile(cstr inPath);

private:
typedef union UColor15Bit{
	u16 u;					// init helper (max 15bit valid 0x7FFF=32767)
	struct{
		u16 r:5;			// red
		u16 g:5;			// green
		u16 b:5;			// blue
		u16 p:1;			// padding garbage, total=16bit
	};
}c15b;
typedef union UColor24Bit{
	struct{
		u16 r:8;			// red
		u16 g:8;			// green
		u16 b:8;			// blue
		u16 a:8;			// alpha
	};
	u32 u;					// init helper, size 32bit
	UColor24Bit&operator=(const UColor15Bit&c){
		r=c.r*8+c.r/4;
		g=c.g*8+c.g/4;
		b=c.b*8+c.b/4;
		return*this;}
	operator cstr(){		// c_str compatibility
		stringstream ss;
		ss	<<hex<<setfill('0')<<right
			<<'_'<<setw(2)<<r<<setw(2)<<g<<setw(2)<<b<<'_';
		RETURN_CONST_C_STR(ss);
	}
}c24b;
typedef struct SVertex{
	f32 x,y,z;bool r;				// rounded (smooth surface)
	SVertex&operator=(const SVertex&v){
		x=v.x;y=v.y;z=v.z;r=v.r;
		return*this;
	}
	SVertex&operator=(const af3&c){
		x=c[0];y=c[1];z=c[2];r=false;
		return*this;
	}
	SVertex operator-(const SVertex&v){
		SVertex a={x-v.x,y-v.y,z-v.z,false};
		return a;
	}
	SVertex operator+(const SVertex&v){
		SVertex a={x+v.x,y+v.y,z+v.z,false};
		return a;
	}
	operator cstr(){				// c_str compatibility
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<x<<';'<<y<<';'<<z<<';';
		static string st[3];		// 3 line buffer
		static u16 i(0);			// track the last one filled
		st[++i%=3]=ss.str();		// fill the next one
		return st[i].c_str();		// emulate const chart pointer return
	}
	f32 dot(const SVertex&v){
		return (x*v.x+y*v.y+z*v.z);
	}
	SVertex cross(const SVertex&v){
		SVertex r=	{y*v.z-z*v.y
					,z*v.x-x*v.z
					,x*v.y-y*v.x};
		return r;
	}
	f32 length(){return sqrtf(x*x+y*y+z*z);}
	void normalize(const string&name,const u16 idx){
		f32 len=length(),mod=1;
		if(len==0)
			cerr<<name<<" V:"<<setw(4)<<right<<idx<<" invalid vertex normal\n";
		else mod=1/len;
		x*=mod;y*=mod;z*=mod;
		return;
	}
	void invert(){x=-x;y=-y;z=-z;}
	bool anyChange(){return x!=0||y!=0||z!=0;}
	SVertex&operator+=(const SVertex&v){
		x+=v.x;y+=v.y;z+=v.z;
		return*this;
	}
	SVertex operator*(const f32&f){
		SVertex n={x*f,y*f,z*f,false};
		return n;
	}
	bool operator!=(const SVertex&v){return (x!=v.x||y!=v.y||z!=v.z);}
	bool operator!=(const af3&v){return (x!=v[0]||y!=v[1]||z!=v[2]);}
}vertex;
typedef vector<vertex>::iterator itvV;
typedef struct SFaceIdx{
	vector<u16>vfi;
	void reverseContent(){
		vector<u16>r(vfi.rbegin(),vfi.rend());
		vfi.swap(r);
	}
	operator cstr(){					// c_str compatibility
		if(vfi.size()==0)return "";
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<vfi.size()<<';';
//		itvic itvic;
		each(it_vu_,vfi)
			ss<<*it_vu_<<',';
		ss.seekp(-1,ss.end)<<';';		// overwrite last char from ',' to ';'
		RETURN_CONST_C_STR(ss);
	}
}faceIdx;
typedef vector<faceIdx>::iterator itvFI;
typedef struct SColor3Float{
	f32 r,g,b;
	SColor3Float&operator=(const c24b&c24){
		r=c24.r/255.f;g=c24.g/255.f;b=c24.b/255.f;
		return*this;
	}
	SColor3Float&operator=(const SColor3Float&c){
		r=c.r;g=c.g;b=c.b;
		return*this;
	}
	SColor3Float&operator=(f32 f){
		r=g=b=f;
		return*this;
	}
	operator cstr(){
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<r<<';'<<g<<';'<<b;
		static string st[3];		// 3 line buffer
		static u16 i(0);			// track the last one filled
		st[++i%=3]=ss.str();		// fill the next one
		return st[i].c_str();		// emulate const chart pointer return
	}
}cl3f;
typedef struct SMaterial{
	u16 i;				// material list index
	string name;
	cl3f d;				// diffuse RGB
	f32 a;				// alpha
	f32 g;				// gloss
	cl3f s;				// specular RGB
	cl3f e;				// emissive RGB
	operator cstr(){
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss	<<"Material "<<name<<"{\n"
			<<d<<";"<<a<<";;\n"
			<<g<<";\n"
			<<s<<";;\n"
			<<e<<";;\n}";
		RETURN_CONST_C_STR(ss);
	}
}material;
typedef map<string,material>::iterator itsMT;
typedef struct SMaterialList{
	vector<string>mtIdx;				// material index per vertex
	map<string,material>mtMap;
	map<string,material>*pomtMap;		// pointer to output material collector
	SMaterialList():pomtMap(NULL){}
	SMaterialList&operator=(const SMaterialList&ml){
		mtMap=ml.mtMap;
		mtIdx=ml.mtIdx;
		return*this;
	}
	operator cstr(){
		u32 sz=mtIdx.size();
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss	<<"MeshMaterialList {\n"
			<<mtMap.size()<<";\n"
			<<mtIdx.size()<<";\n";
		u16 i=0;
// 		itmt itmt;
		each(itsMT,mtMap)
			itsMT->second.i=i++;		// re-indexing all mat idx
		each(it_vs,mtIdx)
			ss<<mtMap[*it_vs].i<<',';
//			ss<<mtMap[*it_vs].i<<",\n";
		ss.seekp(-1,ss.end)<<";;\n";	// overwrite last char from ',' to ";;\n"
//		ss.seekp(-2,ss.end)<<";;\n";	// overwrite last 2 char from ",\n" to ";;\n"
		each(itsMT,mtMap)				// loop each material name
			if(pomtMap)					// true when output material before output mesh
				pomtMap->insert(*itsMT);	// store output material
			else
				ss<<"{"<<itsMT->second.name<<"}\n";
		ss<<"}";
		RETURN_CONST_C_STR(ss);
	}
}mlist;
typedef struct SMeshNormals{
	vector<vertex>vts;					// faces normal coords to vertex normal coords
	vector<faceIdx>fcs;					// redundant faces
	void invertNormal(u16 idx){
		vts[idx].invert();
	}
	void invertFace(u16 idx){
		fcs[idx].reverseContent();
	}
	operator cstr(){
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"MeshNormals{\n"<<vts.size()<<';';
// 		itvv itvv;
		each(itvV,vts)
			ss<<endl<<*itvV<<',';
		ss.seekp(-1,ss.end)<<";\n"<<fcs.size()<<';';	// overwrite last char from ',' to ";\n"
// 		itvf itvf;
		each(itvFI,fcs)
			ss<<endl<<*itvFI<<',';
		ss.seekp(-1,ss.end)<<";\n}";	// overwrite last char from ',' to ";\n"
		RETURN_CONST_C_STR(ss);
	}
}normal;
typedef struct SMesh{
	string name;
	vector<vertex>vts;					// indexed vertex
	vector<faceIdx>fcs;					// Indexed faces
	normal normal;
	mlist mlist;
	vertex*pcnt;						// reference to new mesh center
	map<string,SMesh>*pomhMap;			// pointer to output mesh collector
	map<string,material>*pomtMap;		// pointer to output material collector
	SMesh():pcnt(NULL),pomhMap(NULL),pomtMap(NULL){}
	operator cstr(){
//		if(verts.size()==0)return "";
		if(vts.size()==0){
			stringstream ss;
			ss<<"Mesh "<<name<<"{1;0;0;0;;1;3;0,0,0;;}";
			RETURN_CONST_C_STR(ss);
		};
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"Mesh "<<name<<"{\n"<<vts.size()<<';';
		vector<vertex>v;
// 		itvv itvv;
		if(pcnt&&pcnt->anyChange())			// true when new mesh center
			each(itvV,vts)
				v.push_back((*itvV)-*pcnt);	// apply new center
		else v=vts;
		each(itvV,v)						// each vertex
			ss<<endl<<*itvV<<',';
		ss.seekp(-1,ss.end)<<";\n"<<fcs.size()<<';';
// 		itvf itvf;
		each(itvFI,fcs)						// each face
			ss<<endl<<*itvFI<<',';
		ss.seekp(-1,ss.end)<<";\n"<<mlist<<"\n";
		checkNormal(name);
		ss<<normal<<"\n}";
		RETURN_CONST_C_STR(ss);
	}
	void checkNormal(const string&name){
		if(vts.size()==normal.vts.size())return;
		vertex v={0,0,0,false};			// initializer vertex
		vector<vertex>n(vts.size(),v);	// random access of normal vertex
// 		itvf itvf;						// iter each faces
// 		itvi itvi;						// iter each vertex index
		itvV it=normal.vts.begin();		// group of normal vertex to sum
		each(itvFI,fcs){				// iterate each 144 faces
			each(it_vu_,itvFI->vfi)		// and each 4 or 3 vertex in face
				n[*it_vu_]+=(*it);		// sum normal vertex to n[vertIdx]
			++it;						// next normal vertex
		}
		u16 idx=-1;
		each(itvV,n)					// after sum all normals
			itvV->normalize(name,++idx);			// normalize each
		normal.vts.swap(n);				// save the normalized group
		return;
	}
	vector<u16>listFaceIdx(const string&mt){
		vector<u16>r;
		u16 i=0;						// count faces index
		each(it_vs,mlist.mtIdx){			// mtIdx.size=fcs.size
			if(*it_vs==mt)r.push_back(i);	// true on range 22~44 46~49
			i++;
		}								// r.size 25 faces
		return r;
	}
	vector<u16>listVertIdx(const vector<u16>&fcid){
		map<u16,u16>vfMap;				// store vertex idx and its face count
		u16 i=0;						// count vertex index
		each(it_vu_,fcid){				// each face index
			each(it_vu,fcs[*it_vu_].vfi){	// each vertex index in face
				it_uu it=vfMap.find(*it_vu);	// find vertex before saving
				if(it==vfMap.end())		// true if new vertex index
					vfMap[*it_vu]=1;		// initialize
				else ++vfMap[*it_vu];	// else increase face count
			}
		}
		vector<u16>r;
		each(it_uu,vfMap){
			r.push_back(it_uu->first);
		}
		return r;
	}
	vector<u16>listUsedVertIdx(){
		vector<u16>u(vts.size(),0);		// store how many face use each vertex
		each(itvFI,fcs){				// each face index
			each(it_vu_,itvFI->vfi){		// each vertex index in face
				++u[*it_vu_];			// increase face count
			}
		}
		return u;
	}
	void invertFace(u16 idx){
		fcs[idx].reverseContent();
		normal.invertFace(idx);
	}
	void invertFace(string mt){
		checkNormal(name);
		map<u16,vertex>submh;
		mesh mh;
		itvFI itf=fcs.begin(),itn=normal.fcs.begin();
		itvV  itv=vts.begin();
		each(it_vs_,mlist.mtIdx){		// each string in material index, range 22~44 46~49
			if(*it_vs_==mt){				// true on range 22~44 46~49
				itf->reverseContent();	// reverse face vertex index
				itn->reverseContent();	// reverse normal vertex index
				// need map<vertIdx,vertCoord> of each vert in itf
// 				each(itvuc,(*itn).vfi)	// each 
// 					normal.invertNormal(*itvuc);	//[*itvuc].invert();
				mh.fcs.push_back(*itf);	// face index
				mh.vts.push_back(*itv);	// vertex coords
				
				// normal face index, vertex coords
				itsMT it=mlist.mtMap.find(mt);	// get material
				if(it!=mlist.mtMap.end())
					mh.mlist.mtMap[mt]=it->second;	// copy material
//				mlist.mtMap.erase(it);	// remove material
			}
			++itf;++itn;				// next face, next normal
		}
	}
	SMesh&clear(){
		name.clear();
		vts.clear();
		fcs.clear();
		normal.vts.clear();
		normal.fcs.clear();
		mlist.mtIdx.clear();
		mlist.mtMap.clear();
		pcnt=NULL;
		pomhMap=NULL;
		pomtMap=NULL;
		return*this;
	}

}mesh;
typedef struct SQuaternion{
	f32 w,x,y,z;
	SQuaternion&rad(f32 f){
		const f32 hp(1.5707963f);	// pi/2
		f32 r=hp*f,s;
		s=sinf(r);
		x*=s;y*=s;z*=s;
		w=cosf(r);
//		w=sqrtf(1-x*x-y*y-z*z);
		return*this;
	}
	SQuaternion&ri16(i32 i){		// valid -32786 up to 32768
//		const f32 p=20860.756f;	// 65536/pi
		const f32 r=i/20860.756f;
		f32 s=sinf(r);
		x*=s;y*=s;z*=s;
		w=cosf(r);
//		w=sqrtf(1-x*x-y*y-z*z);
		if(i>32768||i<-32768)w=-w;	// i16 lack of positive 32768 val
		return*this;
	}
	af9 getMatrix(){
		af9 m;
		m[0]=1-2*y*y-2*z*z;m[1]=2*x*y-2*w*z;m[2]=2*x*z+2*w*y;
		m[3]=2*x*y+2*w*z;m[4]=1-2*x*x-2*z*z;m[5]=2*y*z-2*w*x;
		m[6]=2*x*z-2*w*y;m[7]=2*y*z+2*w*x;m[8]=1-2*x*x-2*y*y;
		return m;
	}
	af3 rotateVert(vertex&v){
		vertex q={x,y,z,false},r;
		r=v+(q*2).cross(q.cross(v)+(v*w));
		af3 a={r.x,r.y,r.z};
		return a;
	}
	af3 rotateVert(const af3&c){
		vertex v={c[0],c[1],c[2],false};
		return rotateVert(v);
	}
	SQuaternion&operator=(const vertex&v){
		w=0;
		x=v.x;y=v.y;z=v.z;
		return*this;
	}
	SQuaternion&operator=(const SQuaternion&q){
		w=q.w;x=q.x;y=q.y;z=q.z;
		return*this;
	}
	SQuaternion operator*(const SQuaternion&q){
		SQuaternion r;
		r.w=(w*q.w-x*q.x-y*q.y-z*q.z);
		r.x=(w*q.x+x*q.w+y*q.z-z*q.y);
		r.y=(w*q.y-x*q.z+y*q.w+z*q.x);
		r.z=(w*q.z+x*q.y-y*q.x+z*q.w);
		return r;
	}
	SQuaternion&operator*=(const SQuaternion&q){
		const f32 r[4]=
			{w*q.w-x*q.x-y*q.y-z*q.z
			,x*q.w+w*q.x-z*q.y+y*q.z
			,y*q.w+z*q.x+w*q.y-x*q.z
			,z*q.w-y*q.x+x*q.y+w*q.z};
		w=r[0];x=r[1];y=r[2];z=r[3];
		return*this;
	}
	SQuaternion&normalize(){
		f32 f=w*w+x*x+y*y+z*z,x2,y2,z2;
		i32 i=static_cast<i32>(1000000*f);
		if(i!=1000000){
			f=sqrtf(f);
			x/=f;y/=f;z/=f;
			x2=x*x;y2=y*y;z2=z*z;
			w=sqrtf(1-x2-y2-z2);
		}
		return*this;
	}
	SQuaternion&ai16(const ai3&a){
		SQuaternion t={0,0,1,0},p={0,1,0,0},b={0,0,0,1};
		t.ri16(a[0]);	// turn west/east
		p.ri16(a[1]);	// pitch up/down
		b.ri16(a[2]);	// bank left/right (inverted)
		(*this)=(b*p)*t;
		return*this;
	}
	operator cstr(){
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"4;"<<w<<','<<x<<','<<y<<','<<z<<";;";
		RETURN_CONST_C_STR(ss);
	}
}quat;
typedef struct STransform{
	af9 a;							// angle of rotation
	af3 c;							// center of translation
	STransform&operator=(const STransform&t){
		a=t.a;
		c=t.c;
		return*this;
	}
	STransform&operator=(const af3&p){
		c=p;
		return*this;
	}
	STransform&operator=(const vertex&p){
		c[0]=p.x;c[1]=p.y;c[2]=p.z;
		return*this;
	}
	STransform&operator=(const af9&m){
		a=m;
		return*this;
	}
	STransform&operator=(const ai3&i){
		const f32 p=10430.378350470453f;	// 32768/pi
		const bool keepCenter=true;
		STransform m;
		if(i[2]){					// true if bank angle
			f32 z=i[2]/p;			// bank left/right (left handed)
			a[0]=cosf(z);a[1]=sinf(z);
			a[3]=-sinf(z);a[4]=cosf(z);
			if(i[1]){				// true if bank and pith angle
				m.reset(keepCenter);
				f32 x=i[1]/p;		// pith up/down (right handed)
				m.a[4]=cosf(x);m.a[5]=-sinf(x);
				m.a[7]=sinf(x);m.a[8]=cosf(x);
				(*this)*=m;			// aggregated
			}
			if(i[0]){				// true if bank, pitch and turn angle
				m.reset(keepCenter);
				f32 y=i[0]/p;		// turn around west/east (right handed)
				m.a[0]=cosf(y);m.a[2]=sinf(y);
				m.a[6]=-sinf(y);m.a[8]=cosf(y);
				(*this)*=m;			// aggregated
			}
		}else if(i[1]){				// true if pitch angle
				f32 x=i[1]/p;		// bank left/right (right handed)
				a[4]=cosf(x);a[5]=-sinf(x);
				a[7]=sinf(x);a[8]=cosf(x);
				if(i[0]){			//true if pitch and turn angle
					m.reset(keepCenter);
					f32 y=i[0]/p;	// turn around west/east (right handed)
					m.a[0]=cosf(y);m.a[2]=sinf(y);
					m.a[6]=-sinf(y);m.a[8]=cosf(y);
					(*this)*=m;		// aggregated
				}
		}else if(i[0]){				// true if turn angle only
			f32 y=i[0]/p;			// turn around west/east (right handed)
			a[0]=cosf(y);a[2]=sinf(y);
			a[6]=-sinf(y);a[8]=cosf(y);
		}
		return*this;
	}
	STransform&operator=(quat q){
		a=q.getMatrix();
//		c=q.rotateVert(c);
		return*this;
	}
	STransform operator*(STransform t){
		STransform r;
		r.a[0]=a[0]*t.a[0]+a[1]*t.a[3]+a[2]*t.a[6];
		r.a[1]=a[0]*t.a[1]+a[1]*t.a[4]+a[2]*t.a[7];
		r.a[2]=a[0]*t.a[2]+a[1]*t.a[5]+a[2]*t.a[8];
		r.a[3]=a[3]*t.a[0]+a[4]*t.a[3]+a[5]*t.a[6];
		r.a[4]=a[3]*t.a[1]+a[4]*t.a[4]+a[5]*t.a[7];
		r.a[5]=a[3]*t.a[2]+a[4]*t.a[5]+a[5]*t.a[8];
		r.a[6]=a[6]*t.a[0]+a[7]*t.a[3]+a[8]*t.a[6];
		r.a[7]=a[6]*t.a[1]+a[7]*t.a[4]+a[8]*t.a[7];
		r.a[8]=a[6]*t.a[2]+a[7]*t.a[5]+a[8]*t.a[8];
		r.c=this->c;
		return r;
	}
	STransform&operator*=(const STransform&t){
		(*this)=(*this)*t;
		return*this;
	}
// 	af3 invertCnt(){
// 		af3 r={-c[0],-c[1],-c[2]};
// 		return r;
// 	}
	bool anyChange(){
		return (c[0]||c[1]||c[2]||a[1]||a[2]||a[5]);
	}
	void reset(bool keepCenter=false){
		af9 t={1,0,0,0,1,0,0,0,1};
		a.swap(t);				// reset rotation
		if(!keepCenter)
			c.assign(0);		// reset center position
	}
	operator cstr(){
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"FrameTransformMatrix{\n"
		  <<a[0]<<','<<a[1]<<','<<a[2]<<",0,\n"
		  <<a[3]<<','<<a[4]<<','<<a[5]<<",0,\n"
		  <<a[6]<<','<<a[7]<<','<<a[8]<<",0,\n"
		  <<c[0]<<','<<c[1]<<','<<c[2]<<",1.0;;\n}";
		RETURN_CONST_C_STR(ss);
	}
}mtx;
typedef map<u16,quat>::iterator ituQ;
typedef map<u16,vertex>::iterator ituV;
typedef struct SAnimationKey{			// Animation{
	string name;						// name of frame to animate
	map<u16,quat>agMap;					// map of keys and angles
	map<u16,vertex>mvMap;				// map of keys and movement
	af3 c;								// new center
	u16 cla;							// type of anim
	vector<af3>poss;					// world position coordinates
	vector<ai3>tpbs;					// 3 angles -32768 up to 32768(65536+1)
	vector<bool>disps;					// visible status at animation state coord
	SAnimationKey&operator=(const SAnimationKey&a){	// nested should not copy
		name=a.name;
		c=a.c;
		cla=a.cla;
		poss=a.poss;
		tpbs=a.tpbs;
		disps=a.disps;
		return*this;
	}
	void clear(){
		name.clear();
		agMap.clear();
		mvMap.clear();
		c.fill(0);
		cla=0;
		poss.clear();
		tpbs.clear();
		disps.clear();
	}
	operator cstr(){
		quat q={1,0,0,0};				// init a conventional oriented quat
		cla*=10;
		for(u16 i=0;i<tpbs.size();++i){
			agMap[cla+i]=q.ai16(tpbs[i]);	// turn, pitch and bank animkey
			af3&p=poss[i];
			for(u16 j=0;j<c.size();++j)
				p[j]+=c[j];
			mvMap[cla+i]=p;				// update animkey position
		}
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"AnimationKey{0;\n"
			<<agMap.size()<<";\n";
		// 		ituq ituq;
		each(ituQ,agMap)
			ss<<ituQ->first<<';'<<ituQ->second<<",\n";
		ss.seekp(-2,ss.end);ss<<";\n";	// overwrite last 2 chars from ",\n" to ";\n"
		ss<<"}\nAnimationKey{2;\n"
			<<mvMap.size()<<";\n";
		// 		ituv ituv;
		each(ituV,mvMap)
			ss<<ituV->first<<";3;"
			<<ituV->second.x<<','
			<<ituV->second.y<<','
			<<ituV->second.z<<";;,\n";
		ss.seekp(-2,ss.end);ss<<";\n";	// overwrite last 2 chars from ",\n" to ";\n"
		ss<<"\n}";
		RETURN_CONST_C_STR(ss);
	}
}anikey;
typedef struct SFrame{
	bool nested;			
	string name;
	mtx ftm;							// FrameTransformMatrix
	vertex cnt;							// new center for the nested mesh
	af3 pos;							// world position coordinates
	ai3 tpb;							// 3 angles -32768 up to 32768(65536+1)
	bool disp;							// visible status at still position coord
	string mhId;						// nested mesh
	anikey ak;							// animation of the frame
	vector<string>frIds;				// id of nested frames
	map<string,material>*pomtMap;		// required to output material definition
	map<string,mesh>*pomhMap;			// required to output mesh definition
	map<string,mesh>*pmhMap;			// required to output nested mesh
	map<string,anikey>*pakMap;			// required to output animated frames
	map<string,SFrame>*pfrMap;			// required to output nested frames
	SFrame():nested(false),pomtMap(NULL),pomhMap(NULL),pmhMap(NULL),pakMap(NULL),pfrMap(NULL){
		ftm.reset();
	}
	SFrame&operator=(const SFrame&f){	// nested should not copy
		name=f.name;
 		ftm=f.ftm;
		cnt=f.cnt;
		pos=f.pos;
		tpb=f.tpb;
		disp=f.disp;
		mhId=f.mhId;
		ak=f.ak;
		frIds=f.frIds;
		return*this;
	}
	void clear(){
		name.clear();
		ftm.reset();
		cnt=vertex{0};
		pos.empty();
		tpb.empty();
		disp=false;
		mhId.clear();
		ak.clear();
		frIds.clear();
	}
	operator cstr(){
		if(name=="")
			return"{}";					// true if blacklisted frame
		// animation here
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"Frame "<<name<<"{\n";
		for(u08 i=0;i<3;++i)
			ftm.c[i]+=pos[i];
//		ftm=tpb;						// update rotation matrix
		quat q={1,0,0,0};				// init a conventional oriented quat
		ftm=q.ai16(tpb);				// turn, pitch and bank frame's matrix
		if(ftm.anyChange())				// true when new center/rotation
			ss<<ftm<<endl;				// output
		ak.c=ftm.c;						// update new animation center
		if(pakMap)
			(*pakMap)[name]=ak;			// save current animkey
		if(pmhMap){						// true when output nested mesh
			mesh&mh=(*pmhMap)[mhId],cmh(mh);	// shorcut to mesh, clone mesh
			if(mh.name!="null"){				// true when there is mesh
				if(mh.pcnt&&(*mh.pcnt!=cnt)){	// true when different cnt
//					cmh=mh;
					string s="C~";				// default clone mesh prefix
					s+=cmh.name;				// concatenate name
					if(cmh.name[1]=='~')cmh.name[0]+=1;	// true if clone
					else cmh.name=s;			// else need new name
					(*pmhMap)[cmh.name]=cmh;	// save clone mesh
					mhId=cmh.name;				// output updated clone id instead
					cout<<"different center in Frame: "<<name<<" mesh: "<<cmh.name
						<<"\nbefore: "<<*mh.pcnt
						<<"\naffter: "<<cnt<<"\n\n";
				}
				(*pmhMap)[mhId].pcnt=&cnt;		// update center before output
			}
			if(NESTED_MESH)
				ss<<(*pmhMap)[mhId]<<endl;
			else if(pomhMap)
				(*pomhMap)[mhId]=cmh;
//			m.pcnt=NULL;						// reset mesh center...
		}
		else ss<<'{'<<mhId<<"}\n";
// 		itvsc itvsc;
		each(it_vs_,frIds)						// iterate each nested frame id
 			if(pfrMap){							// true when output nested frame
				frame&f=(*pfrMap)[*it_vs_];
				if(f.name!=""){					// true if not blacklisted
					af3 cmod={-cnt.x,-cnt.y,-cnt.z};
					f.ftm=cmod;					// update new center
					ss<<f<<endl;
				}
			}
 			else ss<<'{'<<*it_vs_<<"}\n";		// last nested frame id
		ss<<"}";
//		ss.str("");///delete
		RETURN_CONST_C_STR(ss);
	}
}frame;
typedef map<string,material>::const_iterator itsMT_;
typedef struct SMapCollMat{
	map<string,material>mtMap;
	SMapCollMat&operator<<(const material&m){
		if(m.name!="")mtMap[m.name]=m;
		return*this;
	}
	SMapCollMat&operator>>(material&m){
		itsMT it=mtMap.begin();
		m=it->second;
		mtMap.erase(it);
		return*this;
	}
	SMapCollMat&operator<<(const mlist&m){
// 		icmt icmt;
		each(itsMT_,mtMap)
			mtMap[itsMT_->first]=itsMT_->second;
		return*this;
	}
	SMapCollMat&operator>>(mlist&m){
		m.mtMap=mtMap;
		mtMap.clear();
		return*this;
	}
	operator cstr(){
		stringstream ss;
// 		itmt itmt;
		each(itsMT,mtMap)
			ss<<itsMT->second<<endl;
		ss<<endl;
		RETURN_CONST_C_STR(ss);
	}
}collMat;
typedef map<string,mesh>::iterator itsMH;
typedef struct SMapCollMsh{
	map<string,mesh>mhMap;
	vector<string>*pmhbl;					// pointer to mesh blacklist
	SMapCollMsh():pmhbl(NULL){}
	SMapCollMsh&operator<<(const mesh&m){
		if(m.name!="")mhMap[m.name]=m;
		return*this;
	}
	SMapCollMsh&operator>>(mesh&m){
		itsMH it=mhMap.begin();
		m=it->second;
		mhMap.erase(it);
		return*this;
	}
	operator cstr(){
		if(pmhbl){								// true when mesh blacklist from ini
// 			itvsc itvsc;
			each(it_vs_,*pmhbl)
				mhMap[*it_vs_].clear();
		}
		stringstream ss;
// 		itmh itmh;
		each(itsMH,mhMap)
			if(itsMH->first!="")
				ss<<itsMH->second<<endl;
		ss<<endl;
		RETURN_CONST_C_STR(ss);
	}
}collMsh;
typedef map<string,frame>::iterator itsFR;
typedef struct SMapCollFrm{
	map<string,frame>frMap;
	map<string,anikey>*pakMap;
	map<string,material>*pomtMap;
	map<string,mesh>*pomhMap;
	map<string,mesh>*pmhMap;
	vector<string>*pmhbl;
	vector<string>*pfrbl;
	vector<string>*pinvfidx;				// invert face by idx
	vector<string>*pinvfmt;					// invert face by material
	SMapCollFrm():pakMap(NULL),pmhMap(NULL),pmhbl(NULL),pfrbl(NULL),pinvfidx(NULL),pinvfmt(NULL){}
	SMapCollFrm&operator<<(const frame&f){
		if(f.name!=""){
			frame&fr=frMap[f.name];			// find where to save
			fr=f;							// save the frame
			if(pakMap)						// true if animkey collector
				fr.pakMap=pakMap;
			if(pomtMap)						// true if output material collector
				fr.pomtMap=pomtMap;
			if(pomhMap)						// true if output mesh collector
				fr.pomhMap=pomhMap;
			if(pmhMap)						// true if mesh collector
				fr.pmhMap=pmhMap;
			fr.pfrMap=&this->frMap;			// share frame collector
		}
		return*this;
	}
	SMapCollFrm&operator>>(frame&f){
		itsFR it=frMap.begin();
		f=it->second;
		frMap.erase(it);
		return*this;
	}
	operator cstr(){
		each(it_vs_,*pinvfidx){				// invert face by idx
			istringstream is(*it_vs_);
			string s;						// get mesh name
			u16 i;							// get face index to invert
			is>>s>>i;
			if(pmhMap)(*pmhMap)[s].invertFace(i);
		}
		each(it_vs_,*pinvfmt){				// invert face by mesh
			istringstream is(*it_vs_);
			string s,mt;
			is>>s>>mt;
			if(pmhMap)(*pmhMap)[s].invertFace(mt);
		}
		if(pmhbl&&pmhMap){					// blacklist mesh
			map<string,mesh>&mhMap=*pmhMap;
// 			itvsc itvsc;
			each(it_vs_,*pmhbl){
				itsMH it=mhMap.find(*it_vs_);
				if(it!=mhMap.end())
					it->second.clear();
				else cout<<"Blacklist Mesh not found: "<<*it_vs_<<endl;
			}
		}
		if(pfrbl){							// blacklist frame
// 			itvsc itvsc;
			each(it_vs_,*pfrbl){
				itsFR it=frMap.find(*it_vs_);
				if(it!=frMap.end())
					it->second.clear();
				else cout<<"Blacklist Frame not found: "<<*it_vs_<<endl;
			}
		}
		stringstream ss;
// 		itfr itfr;
		each(itsFR,frMap)					// loop to find main parent frames
			if(!itsFR->second.nested&&itsFR->second.name!="")	// true if not nested and not empty
				ss<<itsFR->second<<endl;	// output data
		ss<<endl;
		RETURN_CONST_C_STR(ss);
	}
}collFrm;
typedef map<string,anikey>::iterator itsA;
typedef struct SMapCollAnim{
	map<string,anikey>akMap;						// AnimationSet{
	vector<string>*pfrbl;							// pointer to frame blacklist
	SMapCollAnim&operator<<(anikey&ak){
		akMap[ak.name]=ak;
		return*this;
	}
	operator cstr(){
		if(false&&pfrbl){							// blacklist frame
// 			itvsc itvsc;
			each(it_vs_,*pfrbl){
				itsA it=akMap.find(*it_vs_);
				if(it!=akMap.end())
					it->second.clear();
				else cout<<"Blacklist Frame not found: "<<*it_vs_<<endl;
			}
		}
		stringstream ss;
		ss<<FLOAT_PRECISION(6);
		ss<<"AnimationSet{\n";
// 		itsa itsa;
		each(itsA,akMap)
			if(itsA->second.name!="")					// true if not blacklisted
				ss<<"Animation{\n{"<<itsA->first<<"}\n"	// each frame name
					<<itsA->second						// frame's AnimationKey
					<<"\nAnimationOptions{0;0;}\n}\n";	// footer
		ss<<"}\n";
		RETURN_CONST_C_STR(ss);
	}
}collAni;

private:
	flib onDnm
		,onPck
		,onSurf
		,onFace
		,isBright
		,onSrf;							// inside group data status
	f32 scale;							// scale mesh modifier
	string inFilePath;
	collMat mts,omts;					// material collector and output
	collMsh mhs,omhs;					// mesh collector and output
	collFrm frs;
	collAni aks;
	vector<string>mhbl;					// blacklist mesh
	vector<string>frbl;					// blacklist frame
	vector<string>invfidx;				// invert face by idx
	vector<string>invfmt;				// invert face by material
};

class coutl{
public:
	~coutl(){cout<<endl;}
	template<class T>
	coutl&operator<<(const T&x){cout<<x;return*this;}
};
#define sout coutl()
// 	coutl()<<"Hello"<<10<<"World";
// 	coutl()<<"Hello"<<20<<"World";

// manipulator to skip any char
template<char C>
std::istream&skip(std::istream&is){
	if((is>>std::ws).peek()==C)is.ignore();
	else is.setstate(std::ios_base::failbit);
	return is;
}// i.e.  istr>>skip<'#'>;
/*/

// Here's a small template I use that just forwards all output to another
// specified streambuf.

template<typename _Elem,typename _Traits>
class basic_my_streambuf:public std::basic_streambuf<_Elem,_Traits>{
	std::basic_streambuf<_Elem,_Traits>*m_stream;
	typedef typename _Traits::int_type int_type;

protected:
	std::streamsize xsputn(const _Elem*s,std::streamsize n){
		return m_stream->sputn(s,n);
	}
	int overflow(int_type c){return m_stream->sputc(c);}

public:
	basic_my_streambuf(std::basic_streambuf<_Elem,_Traits>*stream):m_stream(stream){}
};
typedef basic_my_streambuf<char,    std::char_traits<char   > >  my_streambuf;
typedef basic_my_streambuf<wchar_t, std::char_traits<wchar_t> > wmy_streambuf;
// Then create your own streambuf like this:
my_streambuf buf(cout.rdbuf());
// And then attach it to cout:
cout.rdbuf(&buf);
// Or create a new output stream from it:
ostream sout(&buf);
// I use this, for instance, to create a streambuffer that automatically indents.
// In the OT's case, I'd not forward to another streambuffer, but just store it
// (e.g., in a std::vector of std::string) and have functions to read the final
// lines and display them graphically.


class AddEndl{
public:
	AddEndl(std::ostream&os):_holder(new Holder(os)){}

	template<class T>
	friend std::ostream&operator<<(const AddEndl&l,const T&t){
		return(l._holder->_os)<<t;
	}
private:
	struct Holder{
		Holder(std::ostream&os):_os(os){}
		~Holder(){_os<<std::endl;}
		std::ostream&_os;
	};
	mutable std::shared_ptr<Holder>_holder;
}
// Then you need a function so that you will get a temporary:
AddEndl wrap(std::ostream& os){return AddEndl(os);}
// This should then work:
//wrap(std::cout)<<"Hello";

//*/
#pragma warning(pop)					// end of disableSpecificWarnings
#endif
