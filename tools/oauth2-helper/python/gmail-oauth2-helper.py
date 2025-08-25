#!/usr/bin/env python3
"""
Simple OAuth2 Helper for Gmail SMTP Authentication
This script helps you get OAuth2 tokens for your mail relay system.
"""

import json
import urllib.parse
import urllib.request
import http.server
import socketserver
import webbrowser
import sys
from urllib.parse import parse_qs, urlparse

class OAuth2Helper:
    def __init__(self, client_id, client_secret, redirect_uri="http://localhost:8080"):
        self.client_id = client_id
        self.client_secret = client_secret
        self.redirect_uri = redirect_uri
        self.auth_code = None
        self.tokens = None
        
    def get_authorization_url(self):
        """Generate the authorization URL for Google OAuth2"""
        params = {
            'client_id': self.client_id,
            'redirect_uri': self.redirect_uri,
            'scope': 'https://mail.google.com/',
            'response_type': 'code',
            'access_type': 'offline',
            'prompt': 'consent'
        }
        
        query_string = urllib.parse.urlencode(params)
        return f"https://accounts.google.com/o/oauth2/v2/auth?{query_string}"
    
    def exchange_code_for_tokens(self, auth_code):
        """Exchange authorization code for access and refresh tokens"""
        data = {
            'client_id': self.client_id,
            'client_secret': self.client_secret,
            'code': auth_code,
            'grant_type': 'authorization_code',
            'redirect_uri': self.redirect_uri
        }
        
        # Encode data for POST request
        encoded_data = urllib.parse.urlencode(data).encode('utf-8')
        
        # Make request to Google's token endpoint
        req = urllib.request.Request(
            'https://oauth2.googleapis.com/token',
            data=encoded_data,
            headers={'Content-Type': 'application/x-www-form-urlencoded'}
        )
        
        try:
            with urllib.request.urlopen(req) as response:
                response_data = response.read().decode('utf-8')
                self.tokens = json.loads(response_data)
                return self.tokens
        except urllib.error.URLError as e:
            print(f"Error exchanging code for tokens: {e}")
            return None

class OAuth2CallbackHandler(http.server.BaseHTTPRequestHandler):
    def __init__(self, *args, oauth2_helper=None, **kwargs):
        self.oauth2_helper = oauth2_helper
        super().__init__(*args, **kwargs)
    
    def do_GET(self):
        """Handle the OAuth2 callback from Google"""
        # Parse the URL to get the authorization code
        parsed_url = urlparse(self.path)
        query_params = parse_qs(parsed_url.query)
        
        if 'code' in query_params:
            auth_code = query_params['code'][0]
            print(f"\n✅ Authorization code received: {auth_code}")
            
            # Exchange code for tokens
            print("🔄 Exchanging authorization code for tokens...")
            tokens = self.oauth2_helper.exchange_code_for_tokens(auth_code)
            
            if tokens:
                print("\n🎉 SUCCESS! Here are your OAuth2 tokens:")
                print("=" * 50)
                print(f"Access Token: {tokens.get('access_token', 'N/A')}")
                print(f"Refresh Token: {tokens.get('refresh_token', 'N/A')}")
                print(f"Token Type: {tokens.get('token_type', 'N/A')}")
                print(f"Expires In: {tokens.get('expires_in', 'N/A')} seconds")
                print("=" * 50)
                
                print("\n💾 IMPORTANT: Save the Refresh Token in your configuration!")
                print("   The Access Token expires in 1 hour, but Refresh Token is long-lived.")
                
                # Save tokens to file
                with open('oauth2_tokens.json', 'w') as f:
                    json.dump(tokens, f, indent=2)
                print("\n💾 Tokens saved to 'oauth2_tokens.json'")
                
                # Send success response
                self.send_response(200)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
                
                html_response = """
                <html>
                <head><title>OAuth2 Success</title></head>
                <body>
                    <h1>✅ OAuth2 Authentication Successful!</h1>
                    <p>Your tokens have been received and saved.</p>
                    <p>You can now close this window and use the refresh token in your configuration.</p>
                </body>
                </html>
                """
                self.wfile.write(html_response.encode())
                
                # Stop the server after successful authentication
                print("\n🛑 Stopping OAuth2 callback server...")
                self.server.shutdown()
            else:
                self.send_error(500, "Failed to exchange code for tokens")
        else:
            self.send_error(400, "No authorization code received")
    
    def log_message(self, format, *args):
        """Suppress HTTP server log messages"""
        pass

def main():
    print("🔐 Gmail OAuth2 Token Helper")
    print("=" * 40)
    
    # Get OAuth2 credentials from user
    client_id = input("Enter your OAuth2 Client ID: ").strip()
    client_secret = input("Enter your OAuth2 Client Secret: ").strip()
    
    if not client_id or not client_secret:
        print("❌ Client ID and Client Secret are required!")
        sys.exit(1)
    
    # Create OAuth2 helper
    oauth2_helper = OAuth2Helper(client_id, client_secret)
    
    # Generate authorization URL
    auth_url = oauth2_helper.get_authorization_url()
    print(f"\n🌐 Authorization URL:")
    print(auth_url)
    
    # Open browser automatically
    print("\n🚀 Opening browser for authorization...")
    try:
        webbrowser.open(auth_url)
    except:
        print("⚠️  Could not open browser automatically. Please copy and paste the URL above.")
    
    print("\n📋 Please authorize the application in your browser.")
    print("   After authorization, you'll be redirected back here.")
    
    # Start callback server
    print("\n🔄 Starting OAuth2 callback server on http://localhost:8080...")
    
    # Create custom handler with OAuth2 helper
    class CustomHandler(OAuth2CallbackHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, oauth2_helper=oauth2_helper, **kwargs)
    
    try:
        with socketserver.TCPServer(("", 8080), CustomHandler) as httpd:
            print("✅ Callback server started. Waiting for authorization...")
            httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n⚠️  Server stopped by user")
    except Exception as e:
        print(f"❌ Error starting server: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
