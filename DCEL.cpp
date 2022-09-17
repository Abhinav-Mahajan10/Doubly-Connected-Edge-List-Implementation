#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<bits/stdc++.h>
#include<cmath>
using namespace std;

//stores coordinates and id of each vertex
class Vertex
{
    public:
        double x, y;
        int id;
        Vertex()
        {
            x = y = 0.0;
            id = 0;
        }
    public:
        void set_vals(double x1, double y1, int id1)
        {
            x = x1;
            y = y1;
            id = id1;
        }
};

//HalfEdge and TwinEdge stores only vertex start and end values and the face it belongs in.
class HalfEdge
{
    public:
        Vertex start, end;
        int face_id;
        HalfEdge()
        {
            face_id = -1;
        }
    public: 
        void set_vals(Vertex start1, Vertex end1, int f)
        {
            start = start1;
            end = end1;
            face_id = f;
        }
};

class TwinEdge
{
    public:
        Vertex start, end;
        int face_id;
        TwinEdge()
        {
            face_id = -1;
        }
    public: 
        void set_vals(Vertex start1, Vertex end1, int f)
        {
            start = start1;
            end = end1;
            face_id = f;
        }
}; 

//Stores the id of the edge. Unique implementation, Edge[id].id = id is always true for this program
//Stores the HalfEdge and TwinEdge 
//Stores the HalfEdge and TwinEdge ids which lead away and to the Edge, this is vital for face wise traversal.
class Edge
{
    public:
        int id;
        HalfEdge Half_Edge;
        TwinEdge Twin_Edge;
        int HalfEdge_next_id, HalfEdge_prev_id, TwinEdge_next_id, TwinEdge_prev_id;
        Edge()
        {
            HalfEdge_next_id = -1;
            HalfEdge_prev_id = -1;
            TwinEdge_next_id = -1;
            TwinEdge_prev_id = -1;
        }
    public:
        void set_vals(int id1, HalfEdge temp1, TwinEdge temp2)
        {
            id = id1;
            Half_Edge = temp1;
            Twin_Edge = temp2;
        }
};

void Split(FILE *, int , int , vector<Edge> &, vector<Vertex> &, int*);

//new functions, to find which face does a particular point belong to
//Algorithm is, from point, start a right direction ray parallel to x axis
//if it intersects even number of points of polygon, it lies outside polygon
//if its odd, then it lies inside
vector<int> find_face(double , double , vector<Edge> &, vector<Vertex> &, vector<vector<int>> &);
bool is_inside_face(double , double , vector<Edge> &, vector<Vertex> &, vector<int> &);
int find_points_of_intersection(double , double , Edge , vector<int> &);
void point_of_intersection(double , double , double , double , double , double , vector<vector<int>> &);
bool inside_line_segment(double , double , double , double , double , double);
double find_distance(double , double , double , double);

//This is used to print the data into the file with pointer fo, 
//Before splitting and after each split this function is called.
vector<vector<int>> Display(FILE *fo, vector<Edge> &Edges, int no_of_faces)
{
    int i;
    //First the Edges are printed.
    fprintf(fo, "Edges:\n");
    for(i = 0;i < Edges.size();i++)
    {
        fprintf(fo, "%d\n", Edges[i].id);
    }

    //WE can collect the face entries of the Edges in the HalfEdge traversal itself,
    //Since we are collecting edges in a, ascending face wise order.
    vector<vector<int>> faces;
    //Now the HalfEdges/TwinEdges belonging to a face, in the order of face id, not edge id.
    fprintf(fo, "HalfEdges:\n");
    for(i = 0;i <= no_of_faces;i++)
    {
        vector<int> face;
        int k;
        for(k = 0;k < Edges.size();k++)
        {
            if(Edges[k].Half_Edge.face_id == i || Edges[k].Twin_Edge.face_id == i)
            {
                break;
            }
        }
        int edge_start_id = k;
        do
        {
            face.push_back(edge_start_id);
            if(Edges[edge_start_id].Half_Edge.face_id == Edges[edge_start_id].Twin_Edge.face_id ||
                Edges[edge_start_id].Half_Edge.face_id == i)
            {
                Edge e = Edges[edge_start_id];
                fprintf(fo, "startVertexIndex=%d endVertexIndex=%d ", e.Half_Edge.start.id, e.Half_Edge.end.id);
                fprintf(fo, "nextEdge=%d previousEdge=%d ", e.HalfEdge_next_id, e.HalfEdge_prev_id);
                fprintf(fo, "faceIndex=%d edgeIndex=%d\n", e.Half_Edge.face_id, e.id);
                edge_start_id = Edges[edge_start_id].HalfEdge_next_id;
            }
            else if(Edges[edge_start_id].Twin_Edge.face_id == i)
            {
                Edge e = Edges[edge_start_id];
                fprintf(fo, "startVertexIndex=%d endVertexIndex=%d ", e.Twin_Edge.start.id, e.Twin_Edge.end.id);
                fprintf(fo, "nextEdge=%d previousEdge=%d ", e.TwinEdge_next_id, e.TwinEdge_prev_id);
                fprintf(fo, "faceIndex=%d edgeIndex=%d\n", e.Twin_Edge.face_id, e.id);
                edge_start_id = Edges[edge_start_id].TwinEdge_next_id;
            }
        } while(edge_start_id != k);
        faces.push_back(face);
    }
    //Lastly, we output the Faces. and what edges belong to it.
    fprintf(fo, "Faces:\n");
    for(i = 0;i < faces.size();i++)
    {
        vector<int> face;
        face = faces[i];
        fprintf(fo, "FaceIndex:%d Edges ", i);
        int k;
        for(k = 0;k < face.size();k++)
        {
            if(k == (face.size() - 1))
            {
                fprintf(fo, "%d\n", face[k]);
            }
            else
            {
                fprintf(fo, "%d->", face[k]);
            }
        }
    }
    fprintf(fo, "\n\n");
    return faces;
}

void print_edges(vector<Edge> &Edges)
{
    int i;
    for(i = 0;i < Edges.size();i++)
    {
        Edge e = Edges[i];
        printf("startVertexIndex=%d endVertexIndex=%d ", e.Half_Edge.start.id, e.Half_Edge.end.id);
        printf("nextEdge=%d previousEdge=%d ", e.HalfEdge_next_id, e.HalfEdge_prev_id);
        printf("faceIndex=%d edgeIndex=%d\n", e.Half_Edge.face_id, e.id);
        printf("startVertexIndex=%d endVertexIndex=%d ", e.Twin_Edge.start.id, e.Twin_Edge.end.id);
        printf("nextEdge=%d previousEdge=%d ", e.TwinEdge_next_id, e.TwinEdge_prev_id);
        printf("faceIndex=%d edgeIndex=%d\n\n", e.Twin_Edge.face_id, e.id);
    }
    printf("\n\n");
}

int main(int argc, char *argv[])
{
    //opening files
    FILE *ff = fopen(argv[1], "r");
    FILE *ff1 = fopen(argv[2], "r");
	FILE *fo = fopen(argv[3], "w");
    //Stores all vertices
    vector<Vertex> vertices;
    //Stores all the edges

    //Lets scan input file
    vector<Edge> Edges;
    int i;
    double x, y;
    int id = 0;
    for(i = 0;i < 4;i++)
    {
        fscanf(ff,"%lf %lf", &x, &y);
        Vertex temp;
        double x1 = double(x);
        double y1 = double(y);
        temp.set_vals(x, y, id++);
        vertices.push_back(temp);
    }

    //Lets initialise the data scanned
    int no_of_faces = 0;
    int no_of_edges = -1;
    for(i = 0;i < 4;i++)
    {
        int vertex_start, vertex_end;
        vertex_start = i;
        vertex_end = (i == 3? 0 : i + 1);
        HalfEdge temp1;
        temp1.set_vals(vertices[vertex_start], vertices[vertex_end], no_of_faces);
        TwinEdge temp2;
        temp2.set_vals(vertices[vertex_end], vertices[vertex_start], no_of_faces);
        Edge temp3;
        temp3.set_vals(++no_of_edges, temp1, temp2);
        Edges.push_back(temp3);
    }

    for(i = 0;i < 4;i++)
    {
        int edge_next, edge_prev;
        edge_next = (i == 3? 0 : i + 1);
        edge_prev = (i == 0? 3 : i - 1);
        Edges[i].HalfEdge_next_id = edge_next;
        Edges[i].HalfEdge_prev_id = edge_prev;
        Edges[i].TwinEdge_next_id = edge_prev;
        Edges[i].TwinEdge_prev_id = edge_next;
    }
    
    //This marks the end of initialisation. Now we print the data we have.
    Display(fo, Edges, no_of_faces);
    
    vector<vector<int>> faces;
    //Now we split and print data for each split.
    char ch[10];
    int x_split, y_split;
    double temp1, temp2;
    while(fscanf(ff1, "%s %lf %lf", ch, &temp1, &temp2) != EOF)
    {
        x_split = (int)temp1;
        y_split = (int)temp2;
        if(ch[0] == 'S')
        {
            Split(ff, x_split, y_split, Edges, vertices, &no_of_faces);
            faces = Display(fo, Edges, no_of_faces);
        }
        else
        {
            //printf("%d %d\n", x_split, y_split);
            //fprintf(fo, "id:%d %d\n", acc++, find_face(x_split, y_split, Edges, vertices, faces));
            fprintf(fo, "Id: ");
            int i;
            vector<int> answer;
            answer = find_face(temp1, temp2, Edges, vertices, faces);
            for(i = 0;i < answer.size() - 1;i++)
            {
                fprintf(fo, "%d, ", answer[i]);
            }
            fprintf(fo, "%d\n", answer[answer.size() - 1]);
        }
    }

    fclose(ff);
    fclose(ff1);
    fclose(fo); 
    return 0;
}

void Split(FILE *ff, int x_split, int y_split, vector<Edge> &Edges, vector<Vertex> &vertices, int *face_no)
{
    //Firstly we have to create a new edge between the edge points of edge's x_split and y_split.
    Edge e1 = Edges[x_split];
    Edge e2 = Edges[y_split];
    double x1, y1;
    x1 = (e1.Half_Edge.start.x + e1.Half_Edge.end.x) / 2;
    y1 = (e1.Half_Edge.start.y + e1.Half_Edge.end.y) / 2;
    double x2, y2;
    x2 = (e2.Half_Edge.start.x + e2.Half_Edge.end.x) / 2;
    y2 = (e2.Half_Edge.start.y + e2.Half_Edge.end.y) / 2;
    Vertex v1, v2;
    v1.set_vals(x1, y1, vertices.size());
    v2.set_vals(x2, y2, vertices.size() + 1);
    vertices.push_back(v1);
    vertices.push_back(v2);

    //lets find common face of the edges
    //This program finds the implementation of only common face splits.
    int face1, face2, face3, face4;
    int common_face = -1;
    face1 = e1.Half_Edge.face_id;
    face2 = e1.Twin_Edge.face_id;
    face3 = e2.Half_Edge.face_id;
    face4 = e2.Twin_Edge.face_id;
    if(face1 == face3 || face1 == face4)
    {
        common_face = face1;
    }
    else if(face2 == face3 || face2 == face4)
    {
        common_face = face2;
    }

    (*face_no)++;

    //We intialise our new Edge and push it in the EdgeList, Edges.
    HalfEdge HE;
    HE.set_vals(v1, v2, common_face);
    TwinEdge TE;
    TE.set_vals(v2, v1, common_face);
    Edge E;
    E.set_vals(Edges.size(), HE, TE);
    E.Twin_Edge.face_id = *face_no;
    Edges.push_back(E);

    //This is for later use.
    //These variables stores the starting and ending edge ids for clockwise traversal from v1 to v2.
    int start_traversal_edge_id = Edges.size();
    int end_traversal_edge_id = Edges.size() + 1;
    //These store whether we first go along the HalfEdge or TwinEdge.
    int flag_HalfEdge = 0;
    int flag_TwinEdge = 0;

    //if the common face is along the HalfEdge, then the HalfEdge is always taken.
    if(e1.Half_Edge.face_id == common_face)
    {
        //WE first make our new Edge for contention case 1, initialise it, then push it in thee Edge List.
        Vertex v_start = e1.Half_Edge.start;
        Vertex v_end = e1.Half_Edge.end;
        HalfEdge HE1;
        HE1.set_vals(v1, v_end, *face_no);
        TwinEdge TE1;
        TE1.set_vals(v_end, v1, *face_no);
        Edge E1;
        E1.set_vals(Edges.size(), HE1, TE1);

        E1.Twin_Edge.face_id = (Edges[x_split].Half_Edge.face_id == Edges[x_split].Twin_Edge.face_id ? *face_no : Edges[x_split].Twin_Edge.face_id);

        E1.HalfEdge_prev_id = Edges.size() - 1;
        E1.HalfEdge_next_id = Edges[x_split].HalfEdge_next_id;
        E1.TwinEdge_next_id = (Edges[x_split].Half_Edge.face_id == Edges[x_split].Twin_Edge.face_id ? Edges.size() - 1 : x_split);
        E1.TwinEdge_prev_id = Edges[x_split].TwinEdge_prev_id;

        Edges.push_back(E1);

        //Now our new vertex E, between midpoints of the plitted face, will get initialised with these edge ids.
        Edges[Edges.size() - 2].TwinEdge_next_id = Edges.size() - 1;
        Edges[Edges.size() - 2].HalfEdge_prev_id = x_split;

        //WE have divided our Edge[x_split] too, so we have to give it new end pointers/ids.
        Edges[x_split].Half_Edge.end = v1;
        Edges[x_split].Twin_Edge.start = v1;

        //We now broadcast to the the neighboring end edges, 
        //That a new edge has been made, am=nd Edge[x_split] is cut short, so they can't access it without the new Edge.
        //Case 1, the The edges leading into Edge[x_split] are the same
        if(Edges[x_split].HalfEdge_next_id == Edges[x_split].TwinEdge_prev_id)
        {
            Edge e = Edges[Edges[x_split].HalfEdge_next_id]; 
            if(e.HalfEdge_prev_id == Edges[x_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;    
            }
            else if(e.HalfEdge_next_id == Edges[x_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[x_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[x_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[x_split].HalfEdge_next_id] = e;
        }
        //Case 2, the edges leading into Edge[x_split] are 2 different edges.
        //This case arises a lot in multiple split examples.
        //A lot of the times an Edges HalfEdge lies in 1 face, and the Twin Edge in another.
        else
        {
            Edge e = Edges[Edges[x_split].HalfEdge_next_id]; 
            if(e.HalfEdge_prev_id == Edges[x_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_next_id == Edges[x_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[x_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[x_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[x_split].HalfEdge_next_id] = e;

            Edge f = Edges[Edges[x_split].TwinEdge_prev_id];
            if(f.HalfEdge_prev_id == Edges[x_split].id)
            {
                f.HalfEdge_prev_id = Edges.size() - 1;
            }
            else if(f.HalfEdge_next_id == Edges[x_split].id)
            {
                f.HalfEdge_next_id = Edges.size() - 1;
            }
            if(f.TwinEdge_next_id == Edges[x_split].id)
            {
                f.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(f.TwinEdge_prev_id == Edges[x_split].id)
            {
                f.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[x_split].TwinEdge_prev_id] = f;
        }

        if(e1.Half_Edge.face_id == e1.Twin_Edge.face_id)
        {
            Edges[x_split].HalfEdge_next_id = Edges.size() - 2;
            Edges[x_split].TwinEdge_prev_id = Edges.size() - 2;
        }
        else
        {
            Edges[x_split].HalfEdge_next_id = Edges.size() - 2;
            Edges[x_split].TwinEdge_prev_id = Edges.size() - 1;
        }

        flag_HalfEdge = 1;
    }
    //Exact same logic applied as before, except this is the case where TwinEdge is taken for cyclic propagation.
    //This might sound stupid, but HalfEdge becomes an arbitary definition when the Edge lies in 2 faces, because it will be clockwise in one, 
    //and anticlockwise in another.
    //This case arises becuase I have implemented WITHOUT making new Edges for each face(duplicate Edges)
    else if(e1.Twin_Edge.face_id == common_face)
    {
        //Initialise and store new vertex
        //And divide Edge[x_split]
        //Same as before, we can even see a mirror effect.
        Vertex v_start = e1.Twin_Edge.start;
        Vertex v_end = e1.Twin_Edge.end;
        HalfEdge HE1;
        HE1.set_vals(v_end, v1, *face_no);
        TwinEdge TE1;
        TE1.set_vals(v1, v_end, *face_no);
        Edge E1;
        E1.set_vals(Edges.size(), HE1, TE1);

        E1.Half_Edge.face_id = (Edges[x_split].Half_Edge.face_id == Edges[x_split].Twin_Edge.face_id ? *face_no : Edges[x_split].Half_Edge.face_id);

        E1.TwinEdge_prev_id = Edges.size() - 1;
        E1.TwinEdge_next_id = Edges[x_split].TwinEdge_next_id;
        E1.HalfEdge_next_id = (Edges[x_split].Half_Edge.face_id == Edges[x_split].Twin_Edge.face_id ? Edges.size() - 1 : x_split);
        E1.HalfEdge_prev_id = Edges[x_split].HalfEdge_prev_id;

        Edges.push_back(E1);

        Edges[Edges.size() - 2].TwinEdge_next_id = Edges.size() - 1;
        Edges[Edges.size() - 2].HalfEdge_prev_id = x_split;

        Edges[x_split].Half_Edge.start = v1;
        Edges[x_split].Twin_Edge.end = v1;

        //Case 1, edges leading into the Edge[x_split] is the same.
        //Then we broadcast the same
        if(Edges[x_split].TwinEdge_next_id == Edges[x_split].HalfEdge_prev_id)
        {
            Edge e = Edges[Edges[x_split].TwinEdge_next_id];
            if(e.HalfEdge_prev_id == Edges[x_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_next_id == Edges[x_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[x_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[x_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[x_split].TwinEdge_next_id] = e;
        }
        //Case 2, the edges are different.
        else
        {
            Edge e = Edges[Edges[x_split].TwinEdge_next_id];
            if(e.HalfEdge_prev_id == Edges[x_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_next_id == Edges[x_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[x_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[x_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[x_split].TwinEdge_next_id] = e;

            Edge f = Edges[Edges[x_split].HalfEdge_prev_id];
            if(f.HalfEdge_prev_id == Edges[x_split].id)
            {
                f.HalfEdge_prev_id = Edges.size() - 1;
            }
            else if(f.HalfEdge_next_id == Edges[x_split].id)
            {
                f.HalfEdge_next_id = Edges.size() - 1;
            }
            if(f.TwinEdge_next_id == Edges[x_split].id)
            {
                f.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(f.TwinEdge_prev_id == Edges[x_split].id)
            {
                f.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[x_split].HalfEdge_prev_id] = f;
        }

        if(e1.Half_Edge.face_id == e1.Twin_Edge.face_id)
        {
            Edges[x_split].TwinEdge_next_id = Edges.size() - 2;
            Edges[x_split].HalfEdge_prev_id = Edges.size() - 2;
        }
        else
        {
            Edges[x_split].TwinEdge_next_id = Edges.size() - 2;
            Edges[x_split].HalfEdge_prev_id = Edges.size() - 1;
        }

        flag_TwinEdge = 1;
    }
    //This marks the end of contention case arising at 1, due to formation of new vertex v1

    //Now we start for contention case 2, arising due to contention case arising because of new Vertex v2
    //Case 1, we are ariving at vertex v2 by HalfEdge
    if(e2.Half_Edge.face_id == common_face)
    {
        //Fomration of new edge followed by splitting Edge[y_split]
        Vertex v_start = e2.Half_Edge.start;
        Vertex v_end = v2;
        HalfEdge HE1;
        HE1.set_vals(v_start, v_end, *face_no);
        TwinEdge TE1;
        TE1.set_vals(v_end, v_start, *face_no);
        TE1.face_id = (Edges[y_split].Half_Edge.face_id == Edges[y_split].Twin_Edge.face_id ? *face_no : Edges[y_split].Twin_Edge.face_id);
        Edge E1;
        E1.set_vals(Edges.size(), HE1, TE1);

        E1.HalfEdge_next_id = Edges.size() - 2;
        E1.HalfEdge_prev_id = Edges[y_split].HalfEdge_prev_id;
        E1.TwinEdge_next_id = Edges[y_split].TwinEdge_next_id;
        E1.TwinEdge_prev_id = (Edges[y_split].Half_Edge.face_id == Edges[y_split].Twin_Edge.face_id ? Edges.size() - 2 : y_split);

        Edges.push_back(E1);
        
        Edges[y_split].Half_Edge.start = v2;
        Edges[y_split].Twin_Edge.end = v2;

        Edges[Edges.size() - 3].TwinEdge_prev_id = Edges.size() - 1;
        Edges[Edges.size() - 3].HalfEdge_next_id = y_split;


        //Case 1, the edges leading into Edges[y_split] are the same
        if(Edges[y_split].TwinEdge_next_id == Edges[y_split].HalfEdge_prev_id)
        {
            Edge e = Edges[Edges[y_split].TwinEdge_next_id];
            if(e.HalfEdge_next_id == Edges[y_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_prev_id == Edges[y_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[y_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[y_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[y_split].TwinEdge_next_id] = e;
        }
        //Case 2, the edges are different
        else
        {
            Edge e = Edges[Edges[y_split].TwinEdge_next_id];
            if(e.HalfEdge_next_id == Edges[y_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_prev_id == Edges[y_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[y_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[y_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[y_split].TwinEdge_next_id] = e;

            Edge f = Edges[Edges[y_split].HalfEdge_prev_id];
            if(f.HalfEdge_next_id == Edges[y_split].id)
            {
                f.HalfEdge_next_id = Edges.size() - 1;
            }
            else if(f.HalfEdge_prev_id == Edges[y_split].id)
            {
                f.HalfEdge_prev_id = Edges.size() - 1;
            }
            if(f.TwinEdge_next_id == Edges[y_split].id)
            {
                f.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(f.TwinEdge_prev_id == Edges[y_split].id)
            {
                f.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[y_split].HalfEdge_prev_id] = f;
        }

        //The remaining info of the divided Edge is given
        if(e2.Half_Edge.face_id == e2.Twin_Edge.face_id)
        {
            Edges[y_split].HalfEdge_prev_id = Edges.size() - 3;
            Edges[y_split].TwinEdge_next_id = Edges.size() - 3;
        }
        else
        {
            Edges[y_split].HalfEdge_prev_id = Edges.size() - 3;
            Edges[y_split].TwinEdge_next_id = Edges.size() - 1;
        }
    }
    //Case 2, we arive at v2 by a TwinEdge
    //Mirror effect again, same things done as before
    else if(e2.Twin_Edge.face_id == common_face)
    {
        //Creation of new Edge and dividing old Edge
        Vertex v_start = e2.Twin_Edge.start;
        Vertex v_end = v2;
        HalfEdge HE1;
        HE1.set_vals(v_end, v_start, *face_no);
        TwinEdge TE1;
        TE1.set_vals(v_start, v_end, *face_no);
        HE1.face_id = (Edges[y_split].Half_Edge.face_id == Edges[y_split].Twin_Edge.face_id ? *face_no : Edges[y_split].Half_Edge.face_id);
        Edge E1;
        E1.set_vals(Edges.size(), HE1, TE1);

        E1.TwinEdge_next_id = Edges.size() - 2;
        E1.TwinEdge_prev_id = Edges[y_split].TwinEdge_prev_id;
        E1.HalfEdge_next_id = Edges[y_split].HalfEdge_next_id;
        E1.HalfEdge_prev_id = (Edges[y_split].Half_Edge.face_id == Edges[y_split].Twin_Edge.face_id ? Edges.size() - 2 : y_split);

        Edges.push_back(E1);
        
        Edges[y_split].Half_Edge.end = v2;
        Edges[y_split].Twin_Edge.start = v2;

        Edges[Edges.size() - 3].TwinEdge_prev_id = Edges.size() - 1;
        Edges[Edges.size() - 3].HalfEdge_next_id = y_split;


        //Case 1, the edges leading into it are the same
        if(Edges[y_split].HalfEdge_next_id == Edges[y_split].TwinEdge_prev_id)
        {
            Edge e = Edges[Edges[y_split].HalfEdge_next_id];
            if(e.HalfEdge_next_id == Edges[y_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_prev_id == Edges[y_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[y_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[y_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[y_split].HalfEdge_next_id] = e;
        }
        //Case 2, the edges are different
        else
        {
            Edge e = Edges[Edges[y_split].HalfEdge_next_id];
            if(e.HalfEdge_next_id == Edges[y_split].id)
            {
                e.HalfEdge_next_id = Edges.size() - 1;
            }
            else if(e.HalfEdge_prev_id == Edges[y_split].id)
            {
                e.HalfEdge_prev_id = Edges.size() - 1;
            }
            if(e.TwinEdge_next_id == Edges[y_split].id)
            {
                e.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(e.TwinEdge_prev_id == Edges[y_split].id)
            {
                e.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[y_split].HalfEdge_next_id] = e;

            Edge f = Edges[Edges[y_split].TwinEdge_prev_id];
            if(f.HalfEdge_next_id == Edges[y_split].id)
            {
                f.HalfEdge_next_id = Edges.size() - 1;
            }
            else if(f.HalfEdge_prev_id == Edges[y_split].id)
            {
                f.HalfEdge_prev_id = Edges.size() - 1;
            }
            if(f.TwinEdge_next_id == Edges[y_split].id)
            {
                f.TwinEdge_next_id = Edges.size() - 1;
            }
            else if(f.TwinEdge_prev_id == Edges[y_split].id)
            {
                f.TwinEdge_prev_id = Edges.size() - 1;
            }
            Edges[Edges[y_split].TwinEdge_prev_id] = f;
        }

        if(e2.Half_Edge.face_id == e2.Twin_Edge.face_id)
        {
            Edges[y_split].HalfEdge_next_id = Edges.size() - 3;
            Edges[y_split].TwinEdge_prev_id = Edges.size() - 3;
        }
        else
        {
            Edges[y_split].TwinEdge_prev_id = Edges.size() - 3;
            Edges[y_split].HalfEdge_next_id = Edges.size() - 1;
        }
    }

    //Now we traverse the clockwise path, and give the required Half and Twin edges new face values.
    if(flag_HalfEdge == 1)
    {
        start_traversal_edge_id = Edges[start_traversal_edge_id].HalfEdge_next_id;
    }
    else if(flag_TwinEdge == 1)
    {
        start_traversal_edge_id = Edges[start_traversal_edge_id].TwinEdge_next_id;
    }
    while(start_traversal_edge_id != end_traversal_edge_id)
    {
        if(Edges[start_traversal_edge_id].Half_Edge.face_id == Edges[start_traversal_edge_id].Twin_Edge.face_id)
        {
            Edges[start_traversal_edge_id].Half_Edge.face_id = *face_no;
            Edges[start_traversal_edge_id].Twin_Edge.face_id = *face_no;
            start_traversal_edge_id = Edges[start_traversal_edge_id].HalfEdge_next_id;
        }
        else if(Edges[start_traversal_edge_id].Half_Edge.face_id == common_face)
        {
            Edges[start_traversal_edge_id].Half_Edge.face_id = *face_no;
            start_traversal_edge_id = Edges[start_traversal_edge_id].HalfEdge_next_id;
        }
        else if(Edges[start_traversal_edge_id].Twin_Edge.face_id == common_face)
        {
            Edges[start_traversal_edge_id].Twin_Edge.face_id = *face_no;
            start_traversal_edge_id = Edges[start_traversal_edge_id].TwinEdge_next_id;
        }
    }
}

//this is a special method to go through all the edges
//and see if the point lies in an edge or not
vector<int> edge_traversal(double x, double y, vector<Edge> &Edges, vector<Vertex> &vertices, vector<vector<int>> &faces)
{
    vector<int> answer;
    int i;
    //vertex flag stores whether (x, y) coincides with a vertex
    bool vertex_flag = false;
    for(i = 0;i < Edges.size();i++)
    {
        Edge e = Edges[i];
        Vertex start = e.Half_Edge.start;
        Vertex end = e.Half_Edge.end;
        double x1 = start.x;
        double y1 = start.y;
        double x2 = end.x;
        double y2 = end.y;
        if((x == x1 && y == y1) || (x == x2 && y == y2))
        {
            vertex_flag = 1;
            int face_id_1 = e.Half_Edge.face_id;
            int face_id_2 = e.Twin_Edge.face_id;
            int flag1 = 1;
            int flag2 = 1;
            for(int k = 0;k < answer.size();k++)
            {
                if(answer[k] == face_id_1)
                {
                    flag1 = 0;
                }
                if(answer[k] == face_id_2)
                {
                    flag2 = 0;
                }
            }
            //we push all the face ids of edges which coincide witht eh vertex (x, y)
            if(flag1 == 1)
            {
                answer.push_back(face_id_1);
            }
            if(flag2 == 1 && face_id_2 != face_id_1)
            {
                answer.push_back(face_id_2);
            }
        }
    }
    //if it was indeed a vertex, then we do not need to traverse the edges
    if(vertex_flag == 1)
    {
        return answer;
    }
    //else, we see if suppose (x, y) lies inside the dge line segment. then we add the face ids which correspond to that edge.
    for(i = 0;i < Edges.size();i++)
    {
        Edge e = Edges[i];
        Vertex start = e.Half_Edge.start;
        Vertex end = e.Half_Edge.end;
        double x1 = start.x;
        double y1 = start.y;
        double x2 = end.x;
        double y2 = end.y;
        bool flag = inside_line_segment(x, y, x1, y1, x2, y2);
        if(flag == true)
        {
            answer.push_back(e.Half_Edge.face_id);
            if(e.Half_Edge.face_id != e.Twin_Edge.face_id)
            {
                answer.push_back(e.Twin_Edge.face_id);
            }
        }
    }
    return answer;
}

//called to find in which face does (x, y) belong
vector<int> find_face(double x, double y, vector<Edge> &Edges, vector<Vertex> &vertices, vector<vector<int>> &faces)
{
    vector<int> answer;
    //first we do edge traversal, if its false, then we travel into the faces, as we are now sure it lies completely inside a face now
    answer = edge_traversal(x, y, Edges, vertices, faces);
    if(answer.size() > 0)
    {
        return answer;
    }
    int i;
    for(i = 0;i < faces.size();i++)
    {
        //for each face, we go and check whether it lies in the face(face is an list of edge ids that constitue the face) or not
        if(is_inside_face(x, y, Edges, vertices, faces[i]))
        {
            answer.push_back(i);
        }
    }
    //if (x, y) belongs to no face, then -1 is the default value returned.
    if(answer.size() == 0)
    {
        answer.push_back(-1);
    }
    return answer;
}

//this exception arises and why it arises will be explained later
bool triangle_exception(double x, double y, vector<Edge> &Edges, vector<Vertex> &vertices, vector<int> &face)
{
    bool flag = false;
    bool answer = false;
    int i;
    int freq = 0;
    Vertex v1;
    //iterate through ther vertices to see if ray intersects a vertex or not
    for(i = 0;i < vertices.size();i++)
    {
        double x_coordinate = vertices[i].x;
        double y_coordinate = vertices[i].y;
        if(y == y_coordinate)
        {
            v1.x = x_coordinate;
            v1.y = y_coordinate;
            freq++;
        }
    }
    //if it intersects only one vertex
    if(freq == 1)
    {
        flag = true;
    }
    if(flag == true)
    {
        //hold the other 2 vertices directly connected by edges in the face provided connected to v1
        vector<Vertex> v_points;
        for(i = 0;i < face.size();i++)
        {
            Edge e = Edges[face[i]];
            Vertex start = e.Half_Edge.start;
            Vertex end = e.Half_Edge.end;
            double x1 = start.x;
            double y1 = start.y;
            double x2 = end.x;
            double y2 = end.y;
            if(y == y1)
            {
                Vertex v_new;
                v_new.x = x2;
                v_new.y = y2;
                v_points.push_back(v_new);
            }
            if(y == y2)
            {
                Vertex v_new;
                v_new.x = x1;
                v_new.y = y1;
                v_points.push_back(v_new);
            }
        }
        //this is the check to see if the point belongs in the face or not when this exception arises
        //if the y of (x, y) is in between the other two vertices, then it belongs to the face
        //else, it does not.
        if(v_points.size() == 2)
        {
            if((y < v_points[0].y && y < v_points[1].y) || (y > v_points[0].y && y > v_points[1].y))
            {
                answer = true;
            }
        }
    }
    return answer;
}

//for a face, its called to see if(x, y) lies inside it
bool is_inside_face(double x, double y, vector<Edge> &Edges, vector<Vertex> &vertices, vector<int> &face)
{
    //from (x,y) we draw a ray parallel to the x axis in the positive direction and find number of points of interscetion with the face(polygon)
    //if its odd, then it lies inside, if its even it lies outside
    bool flag = true;
    int i;
    int points_of_intersection = 0;
    vector<int> addition;
    for(i = 0;i < Edges.size();i++)
    {
        addition.push_back(1);
    }
    for(i = 0;i < face.size();i++)
    {
        points_of_intersection += find_points_of_intersection(x, y, Edges[face[i]], addition);
    }
    //now an exception arises.
    //take the case Split 2 3, id: 4 5
    //when we draw a ray we intersect a vertex.
    //for face 0, it lies inside, for face 1, it lies outside, but points of intersection for both are 3
    //by simple analysis, we take care of this contradicitng example to handle the exception(logical bug)
    if(points_of_intersection == 3)
    {
        bool flag_new = triangle_exception(x, y, Edges, vertices, face);
        if(flag_new == true)
        {
            flag = false;
            return flag;
        }
    }
    if(points_of_intersection % 2 == 0)
    {
        flag = false;
        return flag;
    }
    return flag;
}

int find_points_of_intersection(double x, double y, Edge e, vector<int> &addition)
{
    //now we have an edge e, (x, y) and an array addition whose functionality is explained later.
    int points_of_intersection = 0;
    vector<vector<int>> intersection;
    double x1, x2, y1, y2;
    Vertex start = e.Half_Edge.start;
    Vertex end = e.Half_Edge.end;
    x1 = start.x;
    y1 = start.y;
    x2 = end.x;
    y2 = end.y;
    //we add the intersection vertices to the intersection vector
    point_of_intersection(x, y, x1, y1, x2, y2, intersection);
    int i;
    for(i = 0;i < intersection.size();i++)
    {
        //usually, there is only one point of intersection of a ray and a line
        //but due to edge cases, it may not be so
        //also at times the ray may intersect the edge line segment at a vertex and lie in two edges
        //to make sure the answer is still odd, (number of intersections), we simply add 1 + 2 instead of 1 + 1 by using the addition vector
        double x_intersection = intersection[i][0];
        double y_intersection = intersection[i][1];
        if(x_intersection == x1 && y_intersection == y1)
        {
            points_of_intersection+=(addition[start.id]);
            addition[start.id]++;
        }
        else if(x_intersection == x2 && y_intersection == y2)
        {
            points_of_intersection+=(addition[end.id]);
            addition[end.id]++;
        }
        else
        {
            points_of_intersection++;
        }
    }
    return points_of_intersection;
}

//find the point of intersections of the ray to the edge
void point_of_intersection(double x, double y, double x1, double y1, double x2, double y2, vector<vector<int>> &intersection)
{
    double m;
    //in the case where thee line segment is parallelt o y axis, and slope should NOT be calculated
    if(x2 == x1)
    {
        double x_initial = x;
        double x3 = x1;
        if(inside_line_segment(x3, y, x1, y1, x2, y2) == false || x_initial > x3)
        {
            return ;
        }
        vector<int> point;
        point.push_back(x3);
        point.push_back(y);
        intersection.push_back(point);
        return ;
    }
    m = (y2 - y1) / (x2 - x1);
    double x_initial = x;
    //if slope is 0 and ray is parallel to edge, no point of intersection
    if(m == 0 && y != y2)
    {
        return ;
    }
    //if its parallel but its equal, then are infinte points of intersection
    //so we handle this by various cases suited to our algorithm of odd and even points of intersection
    if(m == 0 && y == y2)
    {
        //if the x is on the left side of both x1 and x2, then we add both the end vertices to the intersection list
        if(x_initial < x1 && x_initial < x2)
        {
            vector<int> point1;
            point1.push_back(x1);
            point1.push_back(y1);
            intersection.push_back(point1);
            vector<int> point2;
            point2.push_back(x2);
            point2.push_back(y2);
            intersection.push_back(point2);
        }
        //in this case we add just one point on the right of it
        else if(x_initial < x1 && x_initial >= x2)
        {
            vector<int> point1;
            point1.push_back(x1);
            point1.push_back(y1);
            intersection.push_back(point1);
        }
        //similar, just the two cases of (x1, y1) or (x2, y2) lying on the right side of the (x, y)
        else if(x_initial < x2 && x_initial >= x1)
        {
            vector<int> point2;
            point2.push_back(x2);
            point2.push_back(y2);
            intersection.push_back(point2);
        }
        return ;
    }
    //this is the point of intersection in the non edge cases scenarios
    double x3 = (y - y1) / m;
    x3 = x3 + x1;
    if(inside_line_segment(x3, y, x1, y1, x2, y2) == false || x_initial > x3) 
    {
        return ;
    }
    vector<int> point;
    point.push_back(x3);
    point.push_back(y);
    intersection.push_back(point);
}

//this simply find whether (x, y) lies within the line segment (x1, y1) extending to (x2, y2)
//using the distance formula
bool inside_line_segment(double x, double y, double x1, double y1, double x2, double y2)
{
    double d1, d2, d3;
    d1 = find_distance(x1, y1, x2, y2);
    d2 = find_distance(x, y, x1, y1);
    d3 = find_distance(x, y, x2, y2);
    double dist_param = (d2 + d3 - d1);
    //in case of irrational values, this buffer values of 0.00001 help us get the correct answer
    if(dist_param <= 0.00001 && dist_param >= -0.00001)
    {
        return true;
    }
    return false;
}

//find distance between (x1, y1) and (x2, y2)
double find_distance(double x1, double y1, double x2, double y2)
{
    double distance = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
    distance = sqrt(distance);
    return distance;
}