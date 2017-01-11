JSONObject jsonPayload;

String aThing="UHI60634";  // our machine that is 'dweeting'.

//Collection variables
int testVal;      
//Collection variable end

int lastm; 

void setup() {
  thread("dweetUpdate");
}


// Thread spins once per second grabbing the payload and filling our local
// data. 
void dweetUpdate() 
{
  int dweet_grab = 0;

  while (true) {
    int m = millis();
    if (m - lastm > 5000) {  // flip dweet_grab 1,0,1,0,0... every second.
      lastm = m;
      dweet_grab =1;
    } else {
      dweet_grab =0;
    }
    // END OF THROTTLE

    if (dweet_grab == 1)
    {
      dweetCollect(aThing);
      try {
        if ( aThing == "UHI60634" ) {  // Change to whatever thing you wish to listen to.
        
        
          //Collection block
          //testVal = jsonPayload.getInt("testVal");
          //Collection block end
          
          //Report block
          println("dweet.io/follow/" + aThing);
          //println("testVal = " + testVal);
          //Report block end
        }
      }
      catch(Exception e) {   
        // If we don't try/catch you'll see missed payload exceptions etc. plus the JSON data can change at any time ...
        println("failed to parse JSON payload or no data available");
      }
    }
  }
}


// dweet 'read'
void dweetCollect(String thing)
{ 
  JSONObject json;

  try {
    json = loadJSONObject("http://dweet.io/get/latest/dweet/for/" + thing );
    JSONArray values = json.getJSONArray("with");
    for ( int i = 0; i < values.size (); i++) {
      JSONObject  item = values.getJSONObject(i);
      String  theThing = item.getString("thing");        
      String   theDate = item.getString("created"); 
      jsonPayload = item.getJSONObject("content"); // extract just the content JSON
      println("dweetCollect(" +theThing +") " +theDate + jsonPayload);
    }
  }
  catch( Exception e ) {
    println("dweetCollect: failed for " + thing);
  }
}