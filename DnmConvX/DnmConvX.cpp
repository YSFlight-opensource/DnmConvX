#include "DnmConvX.h"


CDnmConvX::CDnmConvX(void):omhs(*this),mhs(*this),frs(*this),aks(*this)
{
	nstmt=nstmh=false;
}

// CDnmConvX::~CDnmConvX(void){
// }

CDnmConvX&CDnmConvX::operator<<(istringstream&ss){
	f32 scale(0.01f);						// scale mesh modifier
	flib onDnm(NOT)
		,onPck(NOT)
		,onSurf(NOT)
		,onFace(NOT)
		,isBright(NOT)
		,onSrf(NOT);						// inside group data status
	string line;							// temporal line to parse
	string mtname;							// temporal material name
	string frname;							// temporal frame name
	u32 l=0;								// line index
	++l;getline(ss,line);					// get DNM or SRF header
	if(line=="DYNAMODEL"){
		++onDnm;
		++l;getline(ss,line);				// get DNM version
		if(line[7]!='1')return*this;		
	}else if(line=="SURF")
		++onSurf;
	else return*this;						// wrong file
	material mt={0,"temp-material"
		,{0},1								// diffuse color and Alpha
		,50*1.28f							// glossiness
		,{50/255.f,50/255.f,50/255.f}		// specular color
		,{0}								// emissive color
	};
	mesh mh(*this);								// temp mesh pointer
	while(++l,getline(ss,line)){			// inside mesh
		c15b cl2;							// store 15bit color
		c24b cl4;							// store 24bit color
		u08 t;								// garbage char slot
		string g;							// garbage float slot
		u16 i(0);							// temp int
		istringstream is(line);
		switch(line[0]){
		case'V':{							// Vertex coord or vertex color
			if(onFace){						// true if face vertex id
				is>>t;
				faceIdx fc;
				while(is>>i)fc.vfi.push_back(i);
				mh.fcs.push_back(fc);
				mh.normal.fcs.push_back(fc);	// redundant index
			}else{							// else vertex coords
				vertex v={0,0,0,false};
				is>>t>>v.x>>v.y>>v.z>>t;
				v.r=t=='R';					// round, unused data (for smothing group)
				mh.vts.push_back(v*scale);
			}
			break;}
		case'N':{
			vertex v={0,0,0,false};
			is>>t>>g>>g>>g>>v.x>>v.y>>v.z;
			mh.normal.vts.push_back(v);
			break;}
		case'C':{								// color of the face
			is>>t>>cl2.u;						// assume unsigned 15bit color
			if(is>>i){							// true when handle 24bit color
				cl4.r=cl2.u;					// get red
				cl4.g=i;						// get green
				is>>i;
				cl4.b=i;						// get blue
			}else cl4=cl2;
			break;}
		case'F':{
			++onFace;
			break;}
		case'E':{
			if(onFace--){						// true if end of face
				mtname=cl4;						// create unique rgb name
				mt.d=cl4;
				if(isBright--){					// true if emissive light
					mt.e=mt.d;					// update emissive color
					mtname[0]='G';				// glow indicator
				}else mt.e=4/255.f;				// reset emissive color
				u32 sz=mh.mlist.mtMap.size();	// memo index
				if(mt.name!=mtname||sz==0){		// true if different material
					mt.name=mtname;				// update material
					mt.d=cl4;
					mts<<mt;					// collect material
					mh.mlist.mtMap[mtname]=mt;
				}
				mh.mlist.mtIdx.push_back(mt.name);	// mat index
			}else if(onSurf--){						// else end of mesh
				mtname.clear();
			}
			break;}
		case'S':{								// Header or SubHeader
			if(line=="SURF")++onSurf;
			else if(line.substr(0,3)=="SRF"){	// end of Pck
				++onSrf;
				--onPck;
				is>>g>>frname;
				mhs<<mh;						// mesh done
			}
			break;}
		case'P':{								// start of new surface pack
			if(line.substr(0,3)=="PCK"){
				++onPck;
				if(mh.name!="")					// false at first loop
					mhs<<mh;					// mesh done
				mh.clear();						// room for the new mesh
				is>>g>>g;						// get name, then ignore extension file
				mh.name=g.substr(0,g.find_last_of('.'));
			}
			break;}
		case'B':{								// bright face
			++isBright;							// memorize to apply glow effect
			break;}
		default:{
			break;}
		}
		if(onSrf)
			break;
	}
	frame f(*this);
	anikey&a=f.ak;
	while(++l,getline(ss,line)){				// inside DNM animation
		map<string,frame>&frMap=frs.frMap;		// shortcut for frame map
		istringstream is(line);
		string g;								// temporal string storage
		is>>g;									// get next keyword
		if(g=="SRF"){
			++onSrf;
			is>>frname;							// get frame name
		}else if(g=="FIL"){						// FIL
			is>>g;
			f.mhId=g.substr(0,g.find_last_of('.'));	// set mesh name
		}else if(g=="CLA"){						// CLA
			is>>a.cla;							// type of animation
		}else if(g=="STA"){						// STA
			is>>f.pos[0]>>f.pos[1]>>f.pos[2];
			f32 t=20.f;							// distance treshold
			for(u08 i=0;i<3;++i)
				if(f.pos[i]>t||f.pos[i]<-t)
					f.pos[i]=t;					// limit extreme coords
			for(u08 i=0;i<3;++i)f.pos[i]*=scale;
			is>>f.tpb[0]>>f.tpb[1]>>f.tpb[2];	// turn, then pitch, then bank
			const float e=0.0054931640625f;		// 360/65536 (deg/16bit)
/*			if(f.tpb[0]||f.tpb[1]||f.tpb[2])
				cout<<"Turn,Pitch,-Bank,frame:"<<fixed<<setprecision(0)
					<<setw(4)<<right<<f.tpb[0]*e
					<<setw(4)<<right<<f.tpb[1]*e
					<<setw(4)<<right<<f.tpb[2]*e<<' '<<frname<<endl;
*/			f.tpb[2]*=-1;						// inverted bank angle
			is>>f.disp;							// mesh display at status anim
			a.poss.push_back(f.pos);
			a.tpbs.push_back(f.tpb);
			a.disps.push_back(f.disp);
		}else if(g=="POS"){						// POS
			is>>f.pos[0]>>f.pos[1]>>f.pos[2];
			for(u08 i=0;i<3;++i)f.pos[i]*=scale;
			is>>f.tpb[0]>>f.tpb[1]>>f.tpb[2];	// turn, then pitch, then bank
			const float e=0.0054931640625f;		// 360/65536 (deg/16bit)
/*			if(f.tpb[0]||f.tpb[1]||f.tpb[2])
				cout<<"Turn,Pitch,-Bank,frame:"<<fixed<<setprecision(0)
					<<setw(4)<<right<<f.tpb[0]*e
					<<setw(4)<<right<<f.tpb[1]*e
					<<setw(4)<<right<<f.tpb[2]*e<<' '<<frname<<endl;
*/			f.tpb[2]*=-1;						// inverted bank angle
			is>>f.disp;							// mesh display at still pos
		}else if(g=="CNT"){						// CNT
			is>>f.cnt.x>>f.cnt.y>>f.cnt.z;
			f.cnt=f.cnt*scale;
		}else if(g=="REL"){						// Unknown keyword
			is>>g;
			if(g!="DEP")
				cout<<"line:"<<right<<setw(6)<<l
				<<"different REL in: "<<frname<<endl;
		}else if(g=="CLD"){
			is>>g;
			g=g.substr(1,g.size()-2);			// remove double quot
			frMap[g].nested=true;				// set true on nested child
			f.frIds.push_back(g);				// track nested frame
		}else if(g=="END"){						// End of hierarchy node
			--onSrf;
			f.name=frname.substr(1,frname.size()-2);	// update frame name
			a.name=f.name;						// update frame name to anim
			frs<<f;								// store frame
			f.clear();
		}
	}

	return*this;
}

CDnmConvX::operator cstr(){
	stringstream ss,ss2,ss3;
	ss3<<frs;									// update all frames on output
	ss2<<omhs;									// if any, update all mesh definition
	ss<<"xof 0302txt 0032\n"
		<<"Header{1;0;1;}\n"
		<<"AnimTicksPerSecond{1;}\n\n";			// output D3DX header
	ss<<omts;									// if any, output material definitions
	ss<<ss2.str();								// output mesh definitions
	ss<<ss3.str();								// output nested frames
	ss<<aks;									// output animationkeys
	RETURN_CONST_C_STR(ss);
}
u16 CDnmConvX::outputToXFile(cstr outPath){
	string outFilePath(outPath);
	if(outPath==""){
		outFilePath=inFilePath.substr(0,inFilePath.find_last_of('.'));
		outFilePath+=".x";
	}
	ofstream fout(outFilePath.c_str());
	if(!fout)return E_XWrite;				// error check
	fout<<*this;
	fout.close();
	return E_NotError;
}
u16 CDnmConvX::inputDnmFile(cstr inPath){
	inFilePath=inPath;
	ifstream file(inPath);
	if(!file.is_open())return E_DnmRead;
	string buf;
//	while(!file.eof())buf.push_back(file.get());
	file.seekg(0,file.end);					// go to end of file
	streamoff szf=file.tellg();				// find the file size
	buf.resize(szf+1);						// there is a limit in rezize, 4GB max
	file.seekg(0,file.beg);					// go to begin of file
	file.read(&buf[0],szf);					// populate the string buf, The BAD WAY!
	streamsize szr=file.gcount();
	cout<<"file size\t"<<szf<<endl;
	cout<<"read size\t"<<szr<<endl;
	cout<<"CLRF size\t"<<szf-szr<<endl;
	file.close();
	istringstream ss(buf);
	*this<<ss;
	return E_NotError;
}
u16 CDnmConvX::inputIniFile(cstr inPath){
	string ext(inPath);
	ext=ext.substr(ext.find_last_of('.'),ext.size());
	if(ext!=".ini")return E_IniPath;
	ifstream file(inPath);
	if(!file)return E_IniRead;
	string line;
	vector<string>*bl=NULL;							// shortcut to category
	while(getline(file,line)){
		if(line.size()==0||line[0]=='#')continue;	// skip comment lines
		if(line[0]=='[')bl=NULL;					// init a new category
		if(line=="[Config]")bl=&configs;
		else if(line=="[BlacklistMesh]")bl=&mhbl;
		else if(line=="[BlacklistFrame]")bl=&frbl;
		else if(line=="[InvertFaceByIdx]")bl=&invfidx;
		else if(line=="[InvertFaceByMaterial]")bl=&invfmt;
		else if(bl)bl->push_back(line);				// store config in category
	}
	each(it_vs_,configs)
		if(*it_vs_=="UseNestedMaterial")nstmt=true;
		else if(*it_vs_=="UseNestedMesh")nstmh=true;
	return E_NotError;
}