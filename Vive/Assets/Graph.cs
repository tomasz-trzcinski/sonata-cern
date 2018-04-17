using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using System.IO;
using System.Text;
using System.Linq;


public class Graph : MonoBehaviour {

	int resolution = 50;

	public TextAsset asset;
	string[] pt;
	private ParticleSystem.Particle[] points;
	private ParticleSystem _particleSystem;
	private TracksList tracks;
	private int points_amount;
	private int max_points;
    float deltaTime;

	public string dataFile = "collision";
	string txtContents;

	void Start()
	{
		LoadFile ();
	}
	 
	void LoadFile()
	{
    	TextAsset txtAssets = Resources.Load("collision") as TextAsset;
		this.tracks= JsonUtility.FromJson<TracksList>(txtAssets.text);
		points_amount= 0;
		for(int i=0; i<this.tracks.fTracks.Length;i++){
			points_amount += this.tracks.fTracks[i].fPolyX.Count;
			if (tracks.fTracks [i].fPolyX.Count > max_points)
				max_points = tracks.fTracks [i].fPolyX.Count;
		};
	}

	private void CreatePoints ()
    {

        deltaTime = Time.deltaTime;
		int array_iter = 0;
		points = new ParticleSystem.Particle[points_amount];
		for (int i = 0; i < this.tracks.fTracks.Length; i++) {
            Color color = Random.ColorHSV(0f, 1f, 1f, 1f, 0.5f, 1f);
            List<int> lengthList = new List<int> ();
			lengthList.Add (this.tracks.fTracks [i].fPolyX.Count);
			lengthList.Add (this.tracks.fTracks [i].fPolyY.Count);
			lengthList.Add (this.tracks.fTracks [i].fPolyZ.Count);
			int minVal = lengthList.Min ();
			for (int j = 0; j < minVal; j++) {
				Vector3 position = new Vector3 (this.tracks.fTracks[i].fPolyX[j]/100, this.tracks.fTracks[i].fPolyY[j]/100, this.tracks.fTracks[i].fPolyZ[j]/100);
				points [array_iter].position = position;
				points [array_iter].startColor = color;
				points [array_iter].startSize = 0.1f;
				array_iter++;
			}
		}
	}
	void Update () {
		if (points == null)
			CreatePoints ();
        _particleSystem = gameObject.GetComponent<ParticleSystem> ();
        if (Input.GetKey(KeyCode.LeftArrow))
            _particleSystem.transform.Rotate(Vector3.up * 50 * Time.deltaTime, Space.World);
        if (Input.GetKey(KeyCode.RightArrow))
            _particleSystem.transform.Rotate(Vector3.down * 50 * Time.deltaTime, Space.World);
        if (Input.GetKey(KeyCode.UpArrow))
            _particleSystem.transform.position -= transform.forward/4 + new Vector3(Time.deltaTime, 0, 0);
        if (Input.GetKey(KeyCode.DownArrow))
            _particleSystem.transform.position += transform.forward / 4 + new Vector3(Time.deltaTime, 0, 0);
        _particleSystem.SetParticles (points, points.Length);
	}

}
