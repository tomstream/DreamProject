#include "objstream.h"
#include "globject.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
//#define GLUT_DISABLE_ATEXIT_HACK


#include "glut.h"
#include "glext.h"
//PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
//PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
//PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

using namespace std;

void Obj::read(string file, int id){
	ifstream in(file + ".obj");
	string line;
	int cnt = -1;
	
	cout << "Load obj file: " << file + ".obj" << endl;
	while (getline(in, line)){
		int i = 0;
		while (line[i] == ' ' || line[i] == '\t') ++i;
		line = line.substr(i);
		if (line[0] == 'v'){
			if (line[1] == 't'){
				stringstream ss(line);
				string s;
				float f1, f2;
				ss >> s >> f1 >> f2;
				vtArr.push_back(Vec2f(f1, f2));
			}
			else if (line[1] == 'n'){
				stringstream ss(line);
				string s;
				float f1, f2, f3;
				ss >> s >> f1 >> f2 >> f3;
				vnArr.push_back(Vec3f(f1, f2, f3));
			}
			else if (line[1] == ' ' || line[1] == '\t'){
				stringstream ss(line);
				string s;
				float f1, f2, f3;
				ss >> s >> f1 >> f2 >> f3;
				//sPoint p(f1, f2, f3);
				//objs[cnt].points.push_back(p);
				vArr.push_back(Vec3f(f1, f2, f3));
			}
		}
		else if (line[0] == 'f'){
			int vIndex[3], nIndex[3], tIndex[3];
			string v[3], s;
			stringstream ss(line);
			//cout << ss.str() << endl;
			ss >> s >> v[0] >> v[1] >> v[2];
			//Face f;
			
			sPlane p;
			for (int i = 0; i < 3; ++i){
				sscanf(v[i].c_str(), "%d/%d/%d", &vIndex[i], &tIndex[i], &nIndex[i]);
				p.p[p.psize++] = vIndex[i] - 1;
				p.tex[p.tsize++] = tIndex[i] - 1;
			}
			if (vnArr.size()){
				for (int i = 0; i < 3; ++i)
					p.normal[i] = sPoint(vnArr[nIndex[i] - 1][0], vnArr[nIndex[i] - 1][1], vnArr[nIndex[i] - 1][2]);
			}
			else{
				Vec3f a = vArr[vIndex[1] - 1] - vArr[vIndex[0] - 1];
				Vec3f b = vArr[vIndex[2] - 1] - vArr[vIndex[0] - 1];
				Vec3f n = a * b;
				p.normal[0] = sPoint(n[0], n[1], n[2]);
				p.normal[1] = sPoint(n[0], n[1], n[2]);
				p.normal[2] = sPoint(n[0], n[1], n[2]);
			}
			//calPlane(objs[cnt], p);
			fArr[cnt].push_back(p);
			//objs[cnt].planes[objs[cnt].planeSize++] = p;
			//fArr[cnt].push_back(f);
		}
		stringstream ss(line);
		string s;
		ss >> s;
		if (s == "usemtl"){
			ss >> s;
			textures[s] = Tex();
			names.push_back(s);
			glObject obj;
			obj.points = new sPoint[vArr.size()];
			obj.texPoints = new Vec2f[vtArr.size()];
			for (int i = 0; i < vArr.size(); ++i){
				obj.points[i] = sPoint(vArr[i][0], vArr[i][1], vArr[i][2]);
			}
			for (int i = 0; i < vtArr.size(); ++i){
				obj.texPoints[obj.texSize++] = Vec2f(vtArr[i][0], vtArr[i][1]);
			}
			obj.hasTex = true;
			obj.id = id;
			objs.push_back(obj);
			fArr.push_back(vector<sPlane>());
			cnt++;
		}
	}
	for (int i = 0; i < fArr.size(); ++i){
		objs[i].planes = new sPlane[fArr[i].size()];
		for (int j = 0; j < fArr[i].size(); ++j)
			objs[i].planes[objs[i].planeSize++] = fArr[i][j];
	}
	in.close();
	in.open(file + ".mtl", ios::in);
	Vec3f diffuse, ambient, specular, emission;
	bool hasDiffuse, hasAmbient, hasSpecular, hasEmission;
	string tex;
	while (getline(in, line)){
		int i = 0;
		while (line[i] == ' ' || line[i] == '\t') ++i;
		line = line.substr(i);
		if (line[0] == 'K'){
			stringstream ss(line);
			string s;
			float f1, f2, f3;
			ss >> s >> f1 >> f2 >> f3;
			Vec3f t(f1, f2, f3);
			if (line[1] == 'd'){
				diffuse = t;
			}
			else if (line[1] == 'a'){
				ambient = t;
			}
			else if (line[1] == 's'){
				specular = t;
			}
			else if (line[1] == 'e'){
				emission = t;
			}
		}
		else{
			string t;
			stringstream ss(line);
			ss >> t;
			if (t == "map_Kd"){
				TextureImage tmp;
				ss >> t;
				if (t[t.size() - 1] == 'a')
					LoadTGA(&tmp, t.c_str());
				else if (t[t.size() - 1] == 'p')
					LoadBMP(&tmp, t.c_str());
				cout << "Load " << t << " for texture " << tex << endl;
				for (int i = 0; i < 3; ++i){
					textures[tex].ambient[i] = ambient[i];
					textures[tex].diffuse[i] = diffuse[i];
					textures[tex].specular[i] = specular[i];
					textures[tex].emission[i] = emission[i];
				}
				textures[tex].texture = tmp;
			}
			else if (t == "newmtl"){
				ss >> t;
				tex = t;
			}
		}
	}
	for (int i = 0; i < names.size(); ++i){
		objs[i].tex = textures[names[i]];
		for (int j = 0; j < objs[i].planeSize; ++j){
			calPlane(objs[i], objs[i].planes[j]);
		}
		setConnectivity(objs[i]);
	}

}

void Obj::drawObj(){
	for (int i = 0; i < objs.size(); ++i){

		drawGLObject(objs[i]);
	}
}

void Obj::display(){
	/*glActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
	//GLenum begt = GL_TEXTURE0_ARB;
	GLfloat nothing[3] = { 0.0, 0.0, 0.0 };
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	GLfloat glBlend = 0.75f;
	GLfloat arr[4] = { glBlend, glBlend, glBlend, glBlend };
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	/*glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, arr);*/
	
	
	/*for (int i = 0; i < textures.size(); ++i){
		glActiveTextureARB(begt + i);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[tex].texture.texID);
		
	}*/
	/*for (int k = 0; k < fArr.size(); ++k){
		string tex = names[k];
		vector<Face> &vec = fArr[k];
		if (textures[tex].hasAmbient) glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*)textures[tex].ambient.x);
		if (textures[tex].hasDiffuse) glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*)textures[tex].diffuse.x);
		if (textures[tex].hasSpecular) glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*)textures[tex].specular.x);
		if (textures[tex].hasEmission) glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat*)textures[tex].emission.x);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[tex].texture.texID);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < vec.size(); ++i){
			Vec3f v1 = vArr[vec[i].v[1]] - vArr[vec[i].v[0]];
			Vec3f v2 = vArr[vec[i].v[2]] - vArr[vec[i].v[0]];
			Vec3f norm = v1 * v2;
			for (int j = 0; j < 3; ++j){
				glNormal3f(norm[0], norm[1], norm[2]);
				glTexCoord2f(vtArr[vec[i].t[j]][0], vtArr[vec[i].t[j]][1]);
				//glMultiTexCoord2fARB(begt + k, vtArr[vec[i].t[j]][0], vtArr[vec[i].t[j]][1]);
				glVertex3f(vArr[vec[i].v[j]][0], vArr[vec[i].v[j]][1], vArr[vec[i].v[j]][2]);
			}
		}
		glEnd();

	}
	/*for (int i = 0; i < textures.size(); ++i){
		glActiveTextureARB(begt + i);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[i].texID);
	}*/
	/*glMaterialfv(GL_FRONT, GL_AMBIENT, nothing);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, nothing);
	glMaterialfv(GL_FRONT, GL_SPECULAR, nothing);
	glMaterialfv(GL_FRONT, GL_EMISSION, nothing);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);*/
	for (int i = 0; i < objs.size(); ++i)
		drawGLObject(objs[i]);
}