#!/usr/bin/env python3
"""
Office 365 OAuth2 Helper for Microsoft Graph Authentication
This script helps you get OAuth2 tokens for Office 365 SMTP authentication.
"""

import json
import urllib.parse
import http.server
import socketserver
import webbrowser
import requests
from typing import Dict, Optional

class Office365OAuth2Helper:
    """Helper class for Office 365 OAuth2 authentication"""
    
    def __init__(self, client_id: str, client_secret: str, tenant_id: str):
        self.client_id = client_id
        self.client_secret = client_secret
        self.tenant_id = tenant_id
        self.redirect_uri = "http://localhost:8080"
        self.scope = "https://graph.microsoft.com/Mail.Send"
        
    def get_authorization_url(self) -> str:
        """Generate the authorization URL for Microsoft OAuth2"""
        params = {
            'client_id': self.client_id,
            'response_type': 'code',
            'redirect_uri': self.redirect_uri,
            'scope': self.scope,
            'response_mode': 'query',
            'state': 'office365_oauth2'
        }
        
        query_string = urllib.parse.urlencode(params)
        return f"https://login.microsoftonline.com/{self.tenant_id}/oauth2/v2.0/authorize?{query_string}"
    
    def exchange_code_for_tokens(self, auth_code: str) -> Dict[str, str]:
        """Exchange authorization code for access tokens"""
        token_url = f"https://login.microsoftonline.com/{self.tenant_id}/oauth2/v2.0/token"
        
        data = {
            'client_id': self.client_id,
            'client_secret': self.client_secret,
            'code': auth_code,
            'redirect_uri': self.redirect_uri,
            'grant_type': 'authorization_code'
        }
        
        response = requests.post(token_url, data=data)
        response.raise_for_status()
        
        return response.json()

class OAuth2CallbackHandler(http.server.BaseHTTPRequestHandler):
    """HTTP server to handle OAuth2 callback"""
    
    def __init__(self, *args, oauth2_helper=None, **kwargs):
        self.oauth2_helper = oauth2_helper
        super().__init__(*args, **kwargs)
    
    def do_GET(self):
        """Handle the OAuth2 callback from Microsoft"""
        if self.path.startswith('/?'):
            # Parse query parameters
            query = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(query)
            
            if 'code' in params:
                auth_code = params['code'][0]
                
                try:
                    # Exchange code for tokens
                    tokens = self.oauth2_helper.exchange_code_for_tokens(auth_code)
                    
                    print("\n🎉 SUCCESS! Here are your OAuth2 tokens:")
                    print(f"Access Token: {tokens.get('access_token', 'N/A')}")
                    print(f"Refresh Token: {tokens.get('refresh_token', 'N/A')}")
                    print(f"Token Type: {tokens.get('token_type', 'N/A')}")
                    print(f"Expires In: {tokens.get('expires_in', 'N/A')} seconds")
                    
                    # Save tokens to file
                    with open('office365_oauth2_tokens.json', 'w') as f:
                        json.dump(tokens, f, indent=2)
                    print("\n💾 Tokens saved to 'office365_oauth2_tokens.json'")
                    
                except Exception as e:
                    print(f"\n❌ Error exchanging code for tokens: {e}")
                    self.send_error_response("Authentication failed", str(e))
                    return
                
                # Send success response
                self.send_success_response()
                
                # Stop the server
                self.server.should_stop = True
            else:
                self.send_error_response("No authorization code received", "Missing 'code' parameter")
        else:
            self.send_error_response("Invalid request", "Unexpected path")
    
    def send_success_response(self):
        """Send success HTML response"""
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        
        html = """
        <!DOCTYPE html>
        <html>
        <head><title>OAuth2 Success</title></head>
        <body style="font-family: Arial, sans-serif; text-align: center; padding: 50px;">
            <h1>✅ Office 365 OAuth2 Authentication Successful!</h1>
            <p>You can now close this window and return to the terminal.</p>
            <p>Your tokens have been saved to 'office365_oauth2_tokens.json'</p>
        </body>
        </html>
        """
        self.wfile.write(html.encode())
    
    def send_error_response(self, title: str, message: str):
        """Send error HTML response"""
        self.send_response(400)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        
        html = f"""
        <!DOCTYPE html>
        <html>
        <head><title>OAuth2 Error</title></head>
        <body style="font-family: Arial, sans-serif; text-align: center; padding: 50px;">
            <h1>❌ {title}</h1>
            <p>{message}</p>
            <p>Please check the terminal for more details.</p>
        </body>
        </html>
        """
        self.wfile.write(html.encode())
    
    def log_message(self, format, *args):
        """Suppress HTTP server logging"""
        pass

def main():
    """Main function to run the Office 365 OAuth2 helper"""
    print("🔐 Office 365 OAuth2 Token Helper")
    print("=" * 50)
    
    # Get OAuth2 credentials from user
    client_id = input("Enter your OAuth2 Client ID: ").strip()
    client_secret = input("Enter your OAuth2 Client Secret: ").strip()
    tenant_id = input("Enter your Tenant ID: ").strip()
    
    if not all([client_id, client_secret, tenant_id]):
        print("❌ All fields are required!")
        return
    
    # Create OAuth2 helper
    oauth2_helper = Office365OAuth2Helper(client_id, client_secret, tenant_id)
    
    # Generate authorization URL
    auth_url = oauth2_helper.get_authorization_url()
    
    print(f"\n🔗 Authorization URL generated:")
    print(f"   {auth_url}")
    
    # Ask user to open the URL
    print("\n📋 Please:")
    print("1. Copy the URL above")
    print("2. Open it in your web browser")
    print("3. Sign in with your Office 365 account")
    print("4. Grant the requested permissions")
    print("5. You'll be redirected back to this application")
    
    input("\nPress Enter when you're ready to start the callback server...")
    
    print("\n🔄 Starting OAuth2 callback server on http://localhost:8080...")
    
    # Create custom handler with OAuth2 helper
    class CustomHandler(OAuth2CallbackHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, oauth2_helper=oauth2_helper, **kwargs)
    
    # Create server
    with socketserver.TCPServer(("", 8080), CustomHandler) as httpd:
        httpd.should_stop = False
        
        print("✅ Server started! Waiting for OAuth2 callback...")
        print("🔄 Complete the authentication in your browser...")
        
        try:
            while not httpd.should_stop:
                httpd.handle_request()
        except KeyboardInterrupt:
            print("\n🛑 Stopping OAuth2 callback server...")
        
        print("✅ OAuth2 helper completed successfully!")

if __name__ == "__main__":
    main()
