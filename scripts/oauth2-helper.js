/**
 * Gmail OAuth2 Helper for Website Integration
 * 
 * This script handles the complete OAuth2 flow for Gmail SMTP authentication.
 * Simply include this script in your HTML page and call the functions.
 */

class GmailOAuth2Helper {
    constructor(clientId, clientSecret, redirectUri) {
        this.clientId = clientId;
        this.clientSecret = clientSecret;
        this.redirectUri = redirectUri;
        this.tokens = null;
        this.authCode = null;
    }

    /**
     * Start the OAuth2 authorization flow
     */
    startAuth() {
        // Generate random state for security
        const state = this.generateRandomString(32);
        localStorage.setItem('oauth2_state', state);
        
        // Build authorization URL
        const authUrl = this.buildAuthUrl(state);
        
        // Open popup window for authorization
        const popup = window.open(
            authUrl,
            'gmail_oauth2',
            'width=600,height=700,scrollbars=yes,resizable=yes'
        );
        
        // Listen for the callback
        this.listenForCallback(popup, state);
    }

    /**
     * Build the authorization URL
     */
    buildAuthUrl(state) {
        const params = new URLSearchParams({
            client_id: this.clientId,
            redirect_uri: this.redirectUri,
            response_type: 'code',
            scope: 'https://mail.google.com/',
            access_type: 'offline',
            prompt: 'consent',
            state: state
        });
        
        return `https://accounts.google.com/o/oauth2/v2/auth?${params.toString()}`;
    }

    /**
     * Listen for the OAuth2 callback
     */
    listenForCallback(popup, expectedState) {
        const checkClosed = setInterval(() => {
            if (popup.closed) {
                clearInterval(checkClosed);
                this.handleAuthComplete();
            }
        }, 1000);

        // Listen for message from popup
        window.addEventListener('message', (event) => {
            if (event.origin !== window.location.origin) return;
            
            if (event.data.type === 'oauth2_callback') {
                const { code, state } = event.data;
                
                if (state !== expectedState) {
                    this.showError('Invalid state parameter. Possible CSRF attack.');
                    return;
                }
                
                this.authCode = code;
                this.exchangeCodeForTokens();
                popup.close();
            }
        });
    }

    /**
     * Exchange authorization code for tokens
     */
    async exchangeCodeForTokens() {
        try {
            this.showStatus('Exchanging authorization code for tokens...');
            
            const response = await fetch('https://oauth2.googleapis.com/token', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: new URLSearchParams({
                    client_id: this.clientId,
                    client_secret: this.clientSecret,
                    code: this.authCode,
                    grant_type: 'authorization_code',
                    redirect_uri: this.redirectUri
                })
            });

            if (!response.ok) {
                throw new Error(`Token exchange failed: ${response.status}`);
            }

            this.tokens = await response.json();
            this.showSuccess();
            this.saveTokens();
            
        } catch (error) {
            console.error('Token exchange error:', error);
            this.showError(`Failed to exchange tokens: ${error.message}`);
        }
    }

    /**
     * Save tokens to localStorage (for demo purposes)
     * In production, send these to your server securely
     */
    saveTokens() {
        localStorage.setItem('gmail_oauth2_tokens', JSON.stringify(this.tokens));
        console.log('Tokens saved to localStorage');
    }

    /**
     * Get saved tokens
     */
    getTokens() {
        const saved = localStorage.getItem('gmail_oauth2_tokens');
        return saved ? JSON.parse(saved) : null;
    }

    /**
     * Refresh access token
     */
    async refreshToken() {
        const tokens = this.getTokens();
        if (!tokens || !tokens.refresh_token) {
            throw new Error('No refresh token available');
        }

        try {
            const response = await fetch('https://oauth2.googleapis.com/token', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: new URLSearchParams({
                    client_id: this.clientId,
                    client_secret: this.clientSecret,
                    refresh_token: tokens.refresh_token,
                    grant_type: 'refresh_token'
                })
            });

            if (!response.ok) {
                throw new Error(`Token refresh failed: ${response.status}`);
            }

            const newTokens = await response.json();
            this.tokens = { ...tokens, ...newTokens };
            this.saveTokens();
            
            return this.tokens.access_token;
            
        } catch (error) {
            console.error('Token refresh error:', error);
            throw error;
        }
    }

    /**
     * Generate random string for state parameter
     */
    generateRandomString(length) {
        const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
        let result = '';
        for (let i = 0; i < length; i++) {
            result += chars.charAt(Math.floor(Math.random() * chars.length));
        }
        return result;
    }

    /**
     * Show status message
     */
    showStatus(message) {
        this.updateStatus(message, 'info');
    }

    /**
     * Show success message
     */
    showSuccess() {
        this.updateStatus('✅ OAuth2 authentication successful!', 'success');
        this.displayTokens();
    }

    /**
     * Show error message
     */
    showError(message) {
        this.updateStatus(`❌ ${message}`, 'error');
    }

    /**
     * Update status display
     */
    updateStatus(message, type) {
        const statusEl = document.getElementById('oauth2-status');
        if (statusEl) {
            statusEl.textContent = message;
            statusEl.className = `oauth2-status oauth2-status-${type}`;
        }
        console.log(`[OAuth2] ${message}`);
    }

    /**
     * Display tokens (for demo purposes)
     */
    displayTokens() {
        const tokensEl = document.getElementById('oauth2-tokens');
        if (tokensEl && this.tokens) {
            tokensEl.innerHTML = `
                <h3>OAuth2 Tokens Received:</h3>
                <div class="token-info">
                    <p><strong>Access Token:</strong> <code>${this.tokens.access_token.substring(0, 20)}...</code></p>
                    <p><strong>Refresh Token:</strong> <code>${this.tokens.refresh_token.substring(0, 20)}...</code></p>
                    <p><strong>Expires In:</strong> ${this.tokens.expires_in} seconds</p>
                    <p><strong>Token Type:</strong> ${this.tokens.token_type}</p>
                </div>
                <div class="token-actions">
                    <button onclick="copyRefreshToken()">Copy Refresh Token</button>
                    <button onclick="downloadTokens()">Download Tokens</button>
                </div>
            `;
        }
    }
}

// Global helper functions
function copyRefreshToken() {
    const tokens = JSON.parse(localStorage.getItem('gmail_oauth2_tokens'));
    if (tokens && tokens.refresh_token) {
        navigator.clipboard.writeText(tokens.refresh_token).then(() => {
            alert('Refresh token copied to clipboard!');
        });
    }
}

function downloadTokens() {
    const tokens = localStorage.getItem('gmail_oauth2_tokens');
    if (tokens) {
        const blob = new Blob([tokens], { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'gmail_oauth2_tokens.json';
        a.click();
        URL.revokeObjectURL(url);
    }
}

// Export for use in other scripts
if (typeof module !== 'undefined' && module.exports) {
    module.exports = GmailOAuth2Helper;
}
