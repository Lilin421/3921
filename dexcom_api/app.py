from fastapi import FastAPI, Request

app = FastAPI()

@app.get("/")
def home():
    return {
        "status": "running",
        "message": "Dexcom OAuth callback server"
    }

@app.get("/callback")
def callback(request: Request):
    code = request.query_params.get("code")

    return {
        "authorization_code": code
    }