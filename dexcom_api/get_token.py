import requests

response = requests.post(
    "https://sandbox-api.dexcom.com/v3/oauth2/token",
    headers={
        "Content-Type": "application/x-www-form-urlencoded"
    },
    data={
        "grant_type": "authorization_code",
        "code": "US_87f5536a-19e8-a8b4-8129-7c1efafb7f84.2fc624e6-cb90-2614-b988-2b5de0f34111.a274ac9a-b601-4dff-aed6-58c575454588",
        "redirect_uri": "http://localhost:8000/callback",
        "client_id": "HlKx4pbToHYN3eTQAZZLCbk9zOxUgyvH",
        "client_secret": "Frszdx86BdFRoOJW",
    },
)
# replace code with the actual authorization code received from the Dexcom OAuth callback
print(response.status_code)
print(response.json())