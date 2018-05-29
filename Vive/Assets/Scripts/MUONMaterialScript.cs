using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MUONMaterialScript : MonoBehaviour {
	public Material MUONmaterial;
	private Renderer[] childRenderers;
	// Use this for initialization
	void Start () {
		childRenderers = gameObject.GetComponentsInChildren<Renderer> ();
		foreach (Renderer childRenderer in childRenderers)
			childRenderer.material = MUONmaterial;
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
