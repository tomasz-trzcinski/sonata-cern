using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

[System.Serializable]
public struct FTrack
{
    public int fUniqueID;
    public string fType;
    public int fCharge;
    public float fE;
    public int fParentID;
    public int fPID;
    public float fSignedPT;
    public float fMass;
    public List<float> fMomentum;
    public List<float> fStartCoordinates;
    public List<float> fEndCoordinates;
    public List<float> fChildrenIDs;
    public float fHelixCurvature;
    public float fTheta;
    public float fPhi;
    public List<float> fPolyX;
    public List<float> fPolyY;
    public List<float> fPolyZ;

}


[System.Serializable]
public class TracksList
{
    public FTrack[] fTracks;
}


[System.Serializable]
public class TrackColorList
{
    public TrackColor[] colorMapping;
}

[System.Serializable]
public class TrackColor
{
    public string type;
    public List<int> color;
    public List<int> particleIDs;
}

public class LineScript : MonoBehaviour
{

    int resolution = 50;
    public int track = 0;
    public Material linesMaterial;
    public TextAsset asset;
	public Vector3 newPosition;
    public int speedCollisionTime = 1;
    string[] pt;
    private GameObject[]  childGameObjects;
    private UnityEngine.Object[] textAssets;
    private int collisionIndex = 0; 
    private TracksList tracks;
    private TrackColorList trackColorList;
    private int points_amount;
    private int max_points;
    public float momentum;
    float deltaTime;
	private bool doublePositions= false;
	private int speed = 1;
    private Vector3[] linePositions;
    private float collsionStartEvent;
    private LineRenderer lineRenderer;
	
    public string dataFile = "collision";
    string txtContents;

    // Use this for initialization
    void Start ()
    {
        if (!gameObject.name.Contains("(Clone)"))
        {
            textAssets = Resources.LoadAll("Collisions");
            trackColorList = JsonUtility.FromJson<TrackColorList>((Resources.Load("colors") as TextAsset).text);
            LoadFile();
        }
        else if (gameObject.GetComponent<LineRenderer>().positionCount > 0)
        {
            lineRenderer = gameObject.GetComponent<LineRenderer>();
            linePositions = new Vector3[lineRenderer.positionCount];
            lineRenderer.GetPositions(linePositions);
            lineRenderer.positionCount=0;
        }
    }

    public void LoadFile()
    {
        TextAsset textAsset = textAssets[collisionIndex] as TextAsset;
        tracks = JsonUtility.FromJson<TracksList>(textAsset.text);
        points_amount = 0;
        for (int i = 0; i < tracks.fTracks.Length; i++)
        {
            points_amount += tracks.fTracks[i].fPolyX.Count;
            if (tracks.fTracks[i].fPolyX.Count > max_points)
                max_points = tracks.fTracks[i].fPolyX.Count;
        }
        if (childGameObjects != null)
            for (int i = 0; i < childGameObjects.Length; i++)
                Destroy(childGameObjects[i]);
        childGameObjects = new GameObject[tracks.fTracks.Length];
        for (int i = 0; i < tracks.fTracks.Length; i++)
        {
            childGameObjects[i] = Instantiate(gameObject) as GameObject;
            childGameObjects[i].AddComponent<LineRenderer>();
            if (doublePositions)
                childGameObjects[i].GetComponent<LineScript>().ToggleDoublePositions();
            if (collsionStartEvent != 0)
            {
                childGameObjects[i].GetComponent<LineScript>().RestartCollision();
            }
        };
        if (collisionIndex + 1 == textAssets.Length)
            collisionIndex = 0;
        else
            collisionIndex++;
        CreatePoints();
    }

    private void CreatePoints()
    {
        for (int i = 0; i < this.tracks.fTracks.Length; i++)
        {
            LineRenderer lineRenderer = childGameObjects[i].GetComponent<LineRenderer>();
            List<float> fMomentum = this.tracks.fTracks[i].fMomentum;
            childGameObjects[i].GetComponent<LineScript>().momentum=Mathf.Sqrt(fMomentum[0] * fMomentum[0] + fMomentum[1] * fMomentum[1]);
            lineRenderer.widthMultiplier = 0.06f;
			lineRenderer.numCapVertices = 50;
			lineRenderer.material = linesMaterial;
            float pT = Mathf.Sqrt(tracks.fTracks[i].fMomentum[0] * tracks.fTracks[i].fMomentum[0] + tracks.fTracks[i].fMomentum[1] * tracks.fTracks[i].fMomentum[2]);
            Color color = new Color();
            if (tracks.fTracks[i].fPID == 0)
            {
                color = new Color(255, 0, 255);
            }
            else
                foreach (TrackColor trackColor in trackColorList.colorMapping)
                {
                    if (trackColor.particleIDs.Contains(tracks.fTracks[i].fPID))
                    {
                        color = new Color(trackColor.color[0] / 255f, trackColor.color[1] / 255f, trackColor.color[2] / 255f);
                        break;
                    }
                }
			lineRenderer.material.SetColor("_Color", color);
			lineRenderer.material.SetColor("_EmissionColor", color);
			lineRenderer.generateLightingData = true;
			lineRenderer.useWorldSpace=false;
            List<int> lengthList = new List<int>();
            lengthList.Add(this.tracks.fTracks[i].fPolyX.Count);
            lengthList.Add(this.tracks.fTracks[i].fPolyY.Count);
            lengthList.Add(this.tracks.fTracks[i].fPolyZ.Count);
            int minVal = lengthList.Min();
            if (minVal > 80)
            {
                Debug.Log("NONE");
            }
            lineRenderer.positionCount = minVal;
            for (int j = 0; j < minVal; j++)
            {
                lineRenderer.SetPosition(j, new Vector3(this.tracks.fTracks[i].fPolyX[j] / 125, this.tracks.fTracks[i].fPolyY[j] / 125, this.tracks.fTracks[i].fPolyZ[j] / 125));
            }
        }
    }

    public void RestartCollision()
    {
        collsionStartEvent = Time.time;
    }
    public void ToggleDoublePositions()
    {
        doublePositions = !doublePositions;
    }

    // Update is called once per frame
    void Update () {
        if (Input.GetKeyDown(KeyCode.X))
            ToggleDoublePositions();
        float step = speed * Time.deltaTime;
        if (Input.GetKeyDown(KeyCode.Z))
        {
            RestartCollision();
        }
        if (Input.GetKeyDown(KeyCode.B))
        {
            if (!gameObject.name.Contains("(Clone)"))
                LoadFile();
        }
        int progress;
        progress = Mathf.RoundToInt((Time.time - collsionStartEvent) * momentum * speedCollisionTime);
        if (linePositions != null & momentum!= 0 & collsionStartEvent!= 0)
            if (linePositions.Length > progress)
                {
            int display = progress % linePositions.Length;
            Vector3[] displayedPositions = new Vector3[display];
            Array.Copy(linePositions, displayedPositions, display);
            lineRenderer.positionCount = display;
            lineRenderer.SetPositions(displayedPositions);
        }
		if (doublePositions)
			transform.position = Vector3.MoveTowards(transform.position,newPosition,step);
		else
			transform.position =  Vector3.MoveTowards(transform.position,new Vector3(0,0,0),step);
	}
    
}
