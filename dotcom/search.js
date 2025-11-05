// Implements search functionality

searcher = {
    thisOrg: 'Priory Web',
    /**
     * Class representing a searchable item.
     * The item will be indexed and can be searched based on its keywords.
     */
    Searchable: class {
        /**
         * Construct a Searchable object
         * The object represents an item that can be searched.
         * @param {Object} content - The content of the searchable item, which should be an object with relevant properties.
         *                           Specific properties depend on the type of searchable item, but at least it should have a 'title',
         *                           'subtitle', and 'description' for display purposes.
         * @param {string} type - The type of the searchable item (e.g., 'page', 'file', 'resource', 'data', 'person', 'event').
         * @param {Array<string>} kwds - An array of keywords associated with the searchable item for indexing and search purposes.
         *                               These keywords should be kept in lowercase, and with its simplified form
         *                               (no accents, no s, no past tense etc.) for better search matching.
         */
        constructor (content, type, kwds) {
            this.content = content;
            this.type = type;
            this.keywords = kwds;
        }

        /**
         * Render the search result as an HTML element based on its type
         * @returns {HTMLElement} - The HTML element representing the search result
         */
        render () {
            switch (this.type) {
                case 'file':
                    var _content = this.content;
                    // Set download_url to url if not present
                    if (!_content.download_url && _content.url) {
                        _content.download_url = _content.url;
                        _content.url = null;
                    }
                    // Add tag 'file' to tags array
                    if (!_content.tags) {
                        _content.tags = [];
                    }
                    if (!Array.isArray(_content.tags)) {
                        _content.tags = [_content.tags];
                    }
                    if (!_content.tags.includes('file')) {
                        _content.tags.push('file');
                    }
                    return this.makeHtml(_content, this.type);
                case 'person':
                    var _content = this.content;
                    // Set title to first_name + last_name if not present
                    const firstName = this.content.first_name || '';
                    const lastName = this.content.last_name || '';
                    _content.title = `${firstName} ${lastName}`.trim();
                    // Set subtitle to position if not present
                    if (!_content.subtitle && _content.position) {
                        _content.subtitle = _content.position;
                    }
                    // Add tag person, organization, and position to tags array
                    if (!_content.tags) {
                        _content.tags = [];
                    }
                    if (!Array.isArray(_content.tags)) {
                        _content.tags = [_content.tags];
                    }
                    if (!_content.tags.includes('person')) {
                        _content.tags.push('person');
                    }
                    if (_content.organization && !_content.tags.includes(_content.organization)) {
                        _content.tags.push(_content.organization);
                    }
                    if (_content.position && !_content.tags.includes(_content.position)) {
                        _content.tags.push(_content.position);
                    }
                    return this.makeHtml(_content, this.type);
                case 'event':
                    var _content = this.content;
                    // Construct description from date, time, location, and description if not present
                    let descParts = [];
                    if (_content.date) {
                        descParts.push(`Date: ${_content.date}`);
                    }
                    if (_content.time) {
                        descParts.push(`Time: ${_content.time}`);
                    }
                    if (_content.location) {
                        descParts.push(`Location: ${_content.location}`);
                    }
                    if (_content.description) {
                        descParts.push(_content.description);
                    }
                    _content.description = descParts.join(' | ');
                    // Add tag 'event' to tags array
                    if (!_content.tags) {
                        _content.tags = [];
                    }
                    if (!Array.isArray(_content.tags)) {
                        _content.tags = [_content.tags];
                    }
                    if (!_content.tags.includes('event')) {
                        _content.tags.push('event');
                    }
                    return this.makeHtml(_content, this.type);
                case 'data':
                    var _content = this.content;
                    // Add "Data: " prefix to title if not present
                    if (_content.title && !_content.title.startsWith('Data: ')) {
                        _content.title = `Data: ${_content.title}`;
                    }
                    // Set hosted_by to thisOrg if not present
                    if (!_content.hosted_by) {
                        _content.hosted_by = searcher.thisOrg;
                    }
                    // Add tag 'data' to tags array
                    if (!_content.tags) {
                        _content.tags = [];
                    }
                    if (!Array.isArray(_content.tags)) {
                        _content.tags = [_content.tags];
                    }
                    if (!_content.tags.includes('data')) {
                        _content.tags.push('data');
                    }
                    return this.makeHtml(_content, this.type);
                case 'page':
                    // Add tag 'local' and 'webpage' to tags array
                    var _content = this.content;
                    if (!_content.tags) {
                        _content.tags = [];
                    }
                    if (!Array.isArray(_content.tags)) {
                        _content.tags = [_content.tags];
                    }
                    if (!_content.tags.includes('local')) {
                        _content.tags.push('local');
                    }
                    if (!_content.tags.includes('webpage')) {
                        _content.tags.push('webpage');
                    }
                    return this.makeHtml(_content, this.type);
                case 'resource':
                    // Add tag 'internet' and 'webpage' to tags array
                    var _content = this.content;
                    if (!_content.tags) {
                        _content.tags = [];
                    }
                    if (!Array.isArray(_content.tags)) {
                        _content.tags = [_content.tags];
                    }
                    if (!_content.tags.includes('internet')) {
                        _content.tags.push('internet');
                    }
                    if (!_content.tags.includes('webpage')) {
                        _content.tags.push('webpage');
                    }
                    return this.makeHtml(_content, this.type);
                default:
                    return this.makeHtml(this.content, this.type);
            }
        }

        /**
         * Make HTML representation of the search result, no matter the type
         * @param {Object} content - The content object to render
         * @returns {HTMLElement} - The HTML element representing the search result
         */
        makeHtml (content) {
            // Make a div element
            const _div = document.createElement('div');
            _div.classList.add('search-result');
            // Check if content has id property
            if (content.id) {
                _div.id = `search-result-${content.id}`;
            }
            // Make title link or title block
            const _a_title = document.createElement('h2');
            if (content.url) {
                _a_title.classList.add('search-result-link');
                _a_title.href = content.url;
            } else {
                _a_title.classList.add('search-result-title');
            }
            _a_title.textContent = content.title || type || 'No Title';
            _div.appendChild(_a_title);
            // Make subtitle block
            if (content.subtitle) {
                const _a_subtitle = document.createElement('h3');
                _a_subtitle.classList.add('search-result-subtitle');
                _a_subtitle.textContent = content.subtitle;
                _div.appendChild(_a_subtitle);
            }
            // Make hosted by / owned by / uploaded by block
            if (content.hosted_by || content.owned_by || content.uploaded_by) {
                const _a_by = document.createElement('p');
                _a_by.classList.add('search-result-byline');
                if (content.hosted_by) {
                    _a_by.textContent = ` Hosted by: ${content.hosted_by}`;
                }
                if (content.owned_by) {
                    _a_by.textContent += ` Owned by: ${content.owned_by}`;
                }
                if (content.uploaded_by) {
                    _a_by.textContent += ` Uploaded by: ${content.uploaded_by}`;
                }
                _div.appendChild(_a_by);
            }
            // Make description block
            if (content.description) {
                const _a_description = document.createElement('p');
                _a_description.classList.add('search-result-description');
                _a_description.textContent = content.description;
                _div.appendChild(_a_description);
            }
            // Make download button if applicable
            if (content.download_url) {
                const _a_download = document.createElement('a');
                _a_download.classList.add('search-result-download');
                _a_download.href = content.download_url;
                _a_download.textContent = 'Download';
                _div.appendChild(_a_download);
            }
            // Make tag list if applicable
            if (content.tags && Array.isArray(content.tags) && content.tags.length > 0) {
                const _div_tags = document.createElement('div');
                _div_tags.classList.add('search-result-tags');
                content.tags.forEach(tag => {
                    const _span_tag = document.createElement('span');
                    _span_tag.classList.add('search-result-tag');
                    _span_tag.textContent = tag;
                    _div_tags.appendChild(_span_tag);
                });
                _div.appendChild(_div_tags);
            }
            return _div;
        }

        /**
         * Calculate the Levenshtein distance between two strings
         * @param {string} a - The first string
         * @param {string} b - The second string
         * @returns {number} - The Levenshtein distance between the two strings
         */
        levenshtein(a, b) {
            if (a.length === 0) return b.length;
            if (b.length === 0) return a.length;

            const matrix = Array.from({ length: a.length + 1 }, () => []);
            for (let i = 0; i <= a.length; i++) {
                matrix[i][0] = i;
            }
            for (let j = 0; j <= b.length; j++) {
                matrix[0][j] = j;
            }
            for (let i = 1; i <= a.length; i++) {
                for (let j = 1; j <= b.length; j++) {
                    const cost = a[i - 1] === b[j - 1] ? 0 : 1;
                    matrix[i][j] = Math.min(
                    matrix[i - 1][j] + 1,      // delete
                    matrix[i][j - 1] + 1,      // insert
                    matrix[i - 1][j - 1] + cost // replace
                    );
                }
            }
            return matrix[a.length][b.length];
        }

        /**
         * Calculate a loose similarity score between two strings
         * The score is between 0 and 1, where 1 means identical and 0 means completely different.
         * This function considers both character-level similarity and word-token overlap.
         * @param {string} str1 - The first string
         * @param {string} str2 - The second string
         * @returns {number} - The similarity score between 0 and 1
         */
        looseSimilarity(str1, str2) {
            if (!str1 && !str2) return 1;
            if (!str1 || !str2) return 0;

            str1 = str1.toLowerCase();
            str2 = str2.toLowerCase();

            // Word-token overlap score
            const set1 = new Set(str1.split(/\s+/));
            const set2 = new Set(str2.split(/\s+/));
            const intersection = [...set1].filter(x => set2.has(x)).length;
            const tokenScore = intersection / Math.max(set1.size, set2.size);

            // Character similarity using Levenshtein
            const dist = levenshtein(str1, str2);
            const maxLen = Math.max(str1.length, str2.length);
            const charScore = 1 - dist / maxLen;

            // Weighted average: 70% character score, 30% token score
            return (charScore * 0.7) + (tokenScore * 0.3);
        }

        /**
         * Calculate the total similarity score between the query and the searchable item
         * The score is based on title, subtitle, description, and keywords.
         * @param {string} query - The search query
         * @returns {number} - The total similarity score
         */
        totalSimilarity(query) {
            let totalScore = 0;
            const fieldsToCheck = ['title', 'subtitle', 'description'];
            // Check title, subtitle, and description fields
            fieldsToCheck.forEach(field => {
                if (this.content[field]) {
                    totalScore += this.looseSimilarity(query, this.content[field]);
                }
            });
            // Split query into keywords and check against keywords
            const queryKeywords = query.toLowerCase().split(/\s+/);
            let lo = '';
            queryKeywords.forEach(qk => {
                lo = qk.toLowerCase();
                this.keywords.forEach(kw => {
                    totalScore += this.levenshtein(lo, kw) < 3 ? 1 : 0;
                });
            });
            return totalScore;
        }

        /**
         * Initialize the Searchable object from raw content
         * @param {Object} content - The raw content object
         * @returns {Searchable} - The initialized Searchable object
         */
        fromRawContent(content) {
            this.content = content;
            this.content.keywords = undefined;
            this.content.type = undefined;
            if (content.keywords && Array.isArray(content.keywords)) {
                this.keywords = content.keywords;
            } else {
                this.keywords = [];
            }
            if (content.type) {
                this.type = content.type;
            } else {
                this.type = 'unknown';
            }
            return this;
        }

        /**
         * Initialize the Searchable object from a JSON string
         * @param {string} jsonStr - The JSON string representing the searchable item
         * @returns {Searchable} - The initialized Searchable object
         */
        fromJson(jsonStr) {
            const content = JSON.parse(jsonStr);
            return this.fromRawContent(content);
        }

        /**
         * Initialize the Searchable object from an XML string
         * @param {string} xmlStr - The XML string representing the searchable item
         * @returns {Searchable} - The initialized Searchable object
         */
        fromXML(xmlStr) {
            const parser = new DOMParser();
            const xmlDoc = parser.parseFromString(xmlStr, "application/xml");
            const content = {};
            // Convert XML elements to content object properties
            Array.from(xmlDoc.documentElement.children).forEach(child => {
                content[child.tagName] = child.textContent;
            });
            return this.fromRawContent(content);
        }

        /**
         * Initialize the Searchable object from the compressed data format
         * @param {any} data - The compressed data representing the searchable item
         * @returns {Searchable} - The initialized Searchable object
         */
        fromCompressed(data) {
            
        }
    }
}