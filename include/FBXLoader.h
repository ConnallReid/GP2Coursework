#ifndef FBXLoader_H
#define FBXLoader_H

#include "Common.h"
#include "Mesh.h"
#include "Vertices.h"
#include "GameObject.h"
#include <fbxsdk.h>

shared_ptr<GameObject> loadFBXFromFile(const string& filename);

void processNode(FbxNode *node, shared_ptr<GameObject> parent);
void processAttribute(FbxNodeAttribute * attribute, shared_ptr<GameObject> gameObject);
void processMesh(FbxMesh * mesh, shared_ptr<GameObject> gameObject);
void processMeshTextureCoords(FbxMesh * mesh, Vertex * verts, int numVerts);
void processMeshNormals(FbxMesh * mesh, Vertex * verts, int numVerts);

#endif