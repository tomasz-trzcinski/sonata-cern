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

public class LineScript : MonoBehaviour
{

    int resolution = 50;
    public int track = 0;
    public Material linesMaterial;
    public TextAsset asset;
    string[] pt;
    private GameObject[]  childGameObjects;
    private TracksList tracks;
    private int points_amount;
    private int max_points;
    float deltaTime;

    public string dataFile = "collision";
    string txtContents;

    // Use this for initialization
    void Start ()
    {
        if (!gameObject.name.Contains("(Clone)"))
        {
            LoadFile();
            childGameObjects = new GameObject[this.tracks.fTracks.Length];
            for (int i = 0; i < this.tracks.fTracks.Length; i++)
            {
                childGameObjects[i] = Instantiate(gameObject) as GameObject;
                childGameObjects[i].AddComponent<LineRenderer>();
            };
            CreatePoints();
        }
    }

    void LoadFile()
    {
        TextAsset txtAssets = Resources.Load("collision") as TextAsset;
        this.tracks = JsonUtility.FromJson<TracksList>(txtAssets.text);
        points_amount = 0;
        for (int i = 0; i < this.tracks.fTracks.Length; i++)
        {
            points_amount += this.tracks.fTracks[i].fPolyX.Count;
            if (tracks.fTracks[i].fPolyX.Count > max_points)
                max_points = tracks.fTracks[i].fPolyX.Count;
        };
    }

    private void CreatePoints()
    {
        for (int i = 0; i < this.tracks.fTracks.Length; i++)
        {
            LineRenderer lineRenderer = childGameObjects[i].GetComponent<LineRenderer>();
            lineRenderer.widthMultiplier = 0.06f;
			lineRenderer.numCapVertices = 50;
			lineRenderer.material = linesMaterial;
			Color color = Random.ColorHSV(0f, 1f, 1f, 1f, 0.9f, 1f);
			lineRenderer.material.SetColor("_Color", color);
			lineRenderer.material.SetColor("_EmissionColor", color);
			lineRenderer.generateLightingData = true;
            List<int> lengthList = new List<int>();
            lengthList.Add(this.tracks.fTracks[i].fPolyX.Count);
            lengthList.Add(this.tracks.fTracks[i].fPolyY.Count);
            lengthList.Add(this.tracks.fTracks[i].fPolyZ.Count);
            int minVal = lengthList.Min();
            lineRenderer.positionCount = minVal;
            for (int j = 0; j < minVal; j++)
            {
                lineRenderer.SetPosition(j, new Vector3(this.tracks.fTracks[i].fPolyX[j] / 100, this.tracks.fTracks[i].fPolyY[j] / 100, this.tracks.fTracks[i].fPolyZ[j] / 100));
            }
        }
    }

    // Update is called once per frame
	void Update () {
    }
}
