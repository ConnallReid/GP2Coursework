#include "FBXLoader.h"

int level = 0;

void PrintTabs() {
	for (int i = 0; i < level; i++)
		printf("\t");
}

/**
* Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

shared_ptr<GameObject> loadFBXFromFile(const string& filename)
{
	shared_ptr<GameObject> gameObject = shared_ptr<GameObject>(new GameObject);
	shared_ptr<GameObject> rootGameObject = shared_ptr<GameObject>(new GameObject);
	level = 0;
	// Initialize the SDK manager. This object handles memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Create a new scene so that it can be populated by the imported file.
	if (!lImporter->Initialize(filename.c_str(), -1, lSdkManager->GetIOSettings()))
	{
		return gameObject;
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	FbxGeometryConverter lGeomConverter(lSdkManager);
	lGeomConverter.Triangulate(lScene, /*replace*/true);

	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	FbxNode* lRootNode = lScene->GetRootNode();
	if (lRootNode) {
		cout << "Root Node " << lRootNode->GetName() << endl;
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
		{
			processNode(lRootNode->GetChild(i), rootGameObject);
		}
	}

	lImporter->Destroy();
	return rootGameObject;
}

void processNode(FbxNode *node, shared_ptr<GameObject> parent)
{
	shared_ptr<GameObject> currentGameObject = shared_ptr<GameObject>(new GameObject);
	parent->addChild(currentGameObject);
	PrintTabs();
	const char* nodeName = node->GetName();
	FbxDouble3 translation = node->LclTranslation.Get();
	FbxDouble3 rotation = node->LclRotation.Get();
	FbxDouble3 scaling = node->LclScaling.Get();

	cout << "Node " << nodeName << " Postion " << translation[0] << " " << translation[1] << " " << translation[2] << " "
		<< " Rotation " << rotation[0] << " " << rotation[1] << " " << rotation[2] << " "
		<< " Scale " << scaling[0] << " " << scaling[1] << " " << scaling[2] << endl;

	currentGameObject->setPosition(vec3(translation[0], translation[1], translation[2]));
	currentGameObject->setRotation(vec3(rotation[0], rotation[1], rotation[2]));
	currentGameObject->setScale(vec3(scaling[0], scaling[1], scaling[2]));

	level++;
	// Print the node's attributes.
	for (int i = 0; i < node->GetNodeAttributeCount(); i++){
		processAttribute(node->GetNodeAttributeByIndex(i), currentGameObject);
	}

	// Recursively print the children.
	for (int j = 0; j < node->GetChildCount(); j++)
		processNode(node->GetChild(j), currentGameObject);
	level--;
	PrintTabs();
}

void processAttribute(FbxNodeAttribute * attribute, shared_ptr<GameObject> gameObject)
{
	if (!attribute) return;
	FbxString typeName = GetAttributeTypeName(attribute->GetAttributeType());
	FbxString attrName = attribute->GetName();
	PrintTabs();
	cout << "Attribute " << typeName.Buffer() << " Name " << attrName << endl;
	switch (attribute->GetAttributeType()) {
	case FbxNodeAttribute::eMesh: processMesh(attribute->GetNode()->GetMesh(), gameObject);
	case FbxNodeAttribute::eCamera: return;
	case FbxNodeAttribute::eLight: return;
	}
}

void processMesh(FbxMesh * mesh, shared_ptr<GameObject> gameObject)
{

	int numVerts = mesh->GetControlPointsCount();
	int numIndices = mesh->GetPolygonVertexCount();

	Vertex * pVerts = new Vertex[numVerts];
	int *pIndices = mesh->GetPolygonVertices();

	for (int i = 0; i < numVerts; i++)
	{
		FbxVector4 currentVert = mesh->GetControlPointAt(i);
		pVerts[i].position = vec3(currentVert[0], currentVert[1], currentVert[2]);
		pVerts[i].colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		pVerts[i].texCoords = vec2(0.0f, 0.0f);
	}

	processMeshTextureCoords(mesh, pVerts, numVerts);
	processMeshNormals(mesh, pVerts, numVerts);

	gameObject->createBuffers(pVerts, numVerts, pIndices, numIndices);

	cout << "Vertices " << numVerts << " Indices " << numIndices << endl;


	if (pVerts)
	{
		delete[] pVerts;
		pVerts = NULL;
	}
}


void processMeshTextureCoords(FbxMesh * mesh, Vertex * verts, int numVerts)
{
	for (int iPolygon = 0; iPolygon < mesh->GetPolygonCount(); iPolygon++) {
		for (unsigned iPolygonVertex = 0; iPolygonVertex < 3; iPolygonVertex++) {
			int fbxCornerIndex = mesh->GetPolygonVertex(iPolygon, iPolygonVertex);
			FbxVector2 fbxUV = FbxVector2(0.0, 0.0);
			FbxLayerElementUV* fbxLayerUV = mesh->GetLayer(0)->GetUVs();
			// Get texture coordinate
			if (fbxLayerUV) {
				int iUVIndex = 0;
				switch (fbxLayerUV->GetMappingMode()) {
				case FbxLayerElement::eByControlPoint:
					iUVIndex = fbxCornerIndex;
					break;
				case FbxLayerElement::eByPolygonVertex:
					iUVIndex = mesh->GetTextureUVIndex(iPolygon, iPolygonVertex, FbxLayerElement::eTextureDiffuse);
					break;
				case FbxLayerElement::eByPolygon:
					iUVIndex = iPolygon;
					break;
				}
				fbxUV = fbxLayerUV->GetDirectArray().GetAt(iUVIndex);
				verts[fbxCornerIndex].texCoords.x = fbxUV[0];
				verts[fbxCornerIndex].texCoords.y = 1.0f - fbxUV[1];
			}
		}
	}
}

void processMeshNormals(FbxMesh * mesh, Vertex * verts, int numVerts)
{
	for (int iPolygon = 0; iPolygon < mesh->GetPolygonCount(); iPolygon++) {
		for (unsigned iPolygonVertex = 0; iPolygonVertex < 3; iPolygonVertex++) {
			int fbxCornerIndex = mesh->GetPolygonVertex(iPolygon, iPolygonVertex);
			FbxVector4 fbxNormal;
			mesh->GetPolygonVertexNormal(iPolygon, iPolygonVertex, fbxNormal);
			fbxNormal.Normalize();
			verts[fbxCornerIndex].normal.x = fbxNormal[0];
			verts[fbxCornerIndex].normal.y = fbxNormal[1];
			verts[fbxCornerIndex].normal.z = fbxNormal[2];
		}
	}
}